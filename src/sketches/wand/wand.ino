#include <hcxing-project-1_inferencing.h>

/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 * Licensed under the Apache License, Version 2.0
 */


#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Pin assignments
#define RED_LED 21
#define GREEN_LED 20
#define BLUE_LED 8
#define BUTTON_PIN 3



Adafruit_MPU6050 mpu;

#define SAMPLE_RATE_MS 10
#define CAPTURE_DURATION_MS 1000
#define FEATURE_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE

bool capturing = false;
unsigned long last_sample_time = 0;
unsigned long capture_start_time = 0;
int sample_count = 0;
float features[FEATURE_SIZE];

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

void light_led(const char* prediction) {
    Serial.print("Trying to light: [");
    Serial.print(prediction);
    Serial.println("]");

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    if (strstr(prediction, "Z")) {
        Serial.println("→ RED ON");
        digitalWrite(RED_LED, HIGH);
    } else if (strstr(prediction, "o") || strstr(prediction, "O")) {
        Serial.println("→ GREEN ON");
        digitalWrite(GREEN_LED, HIGH);
    } else if (strstr(prediction, "v") || strstr(prediction, "V")) {
        Serial.println("→ BLUE ON");
        digitalWrite(BLUE_LED, HIGH);
    } else {
        Serial.println("→ No matching prediction");
}

}




void print_inference_result(ei_impulse_result_t result) {
    Serial.println("✅ I got into the print_inference_result!");

    float max_value = 0;
    int max_index = -1;
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_value) {
            max_value = result.classification[i].value;
            max_index = i;
        }
    }

    const char* prediction = "0";  // 默认是 o
    if (max_index != -1) {
        prediction = ei_classifier_inferencing_categories[max_index];
    }

    Serial.print("Prediction: ");
    Serial.print(prediction);
    Serial.print(" (");
    Serial.print(max_value * 100);
    Serial.println("%)");

    Serial.print("Prediction raw hex: ");
    for (int i = 0; i < strlen(prediction); i++) {
        Serial.print("0x");
        Serial.print((byte)prediction[i], HEX);
        Serial.print(" ");
    }
    Serial.println();


    light_led(prediction);  // 点亮对应颜色
    delay(3000);            // 保持亮灯 3 秒
    light_led("");          // 关闭灯（传入空字符串无匹配 => 全灭）
}



void run_inference() {
    if (sample_count * 3 < EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        Serial.println("WARNING: Not enough data, default to 'o'");
        light_led("o");
        return;
    }

    ei_impulse_result_t result = { 0 };
    signal_t features_signal;
    features_signal.total_length = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE;
    features_signal.get_data = &raw_feature_get_data;

    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false);
    if (res != EI_IMPULSE_OK) {
        Serial.print("ERR: Failed to run classifier (");
        Serial.print(res);
        Serial.println(")");
        light_led("o");
        return;
    }

    print_inference_result(result);
}

void capture_accelerometer_data() {
    if (millis() - last_sample_time >= SAMPLE_RATE_MS) {
        last_sample_time = millis();
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        if (sample_count < FEATURE_SIZE / 3) {
            int idx = sample_count * 3;
            features[idx] = a.acceleration.x;
            features[idx + 1] = a.acceleration.y;
            features[idx + 2] = a.acceleration.z;
            sample_count++;
        }

        if (millis() - capture_start_time >= CAPTURE_DURATION_MS) {
            capturing = false;
            Serial.println("Capture complete");
            run_inference();
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);

    Serial.println("Initializing MPU6050...");
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) delay(10);
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    Serial.println("MPU6050 initialized successfully");
    Serial.println("Send 'o' or press button to start gesture capture");
}

void loop() {
    if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == 'o') {
            Serial.println("Starting gesture capture (serial)...");
            sample_count = 0;
            capturing = true;
            capture_start_time = millis();
            last_sample_time = millis();
        }
    }

    if (digitalRead(BUTTON_PIN) == LOW && !capturing) {
        Serial.println("Starting gesture capture (button)...");
        delay(200); // debounce
        sample_count = 0;
        capturing = true;
        capture_start_time = millis();
        last_sample_time = millis();
    }

    if (capturing) {
        capture_accelerometer_data();
    }
}

