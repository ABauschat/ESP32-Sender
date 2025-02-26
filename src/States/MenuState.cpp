#include "MenuState.h"
#include "HandleEvents.h"
#include "Application.h"
#include "StateFactory.h"
#include "Colors.h"
#include "Device.h"
#include <Arduino.h>
#include <Wire.h>
#include "driver/mcpwm.h"   // Include the ESP32 MCPWM driver
#include "HX711.h"          // Ensure HX711 header is included

// Define the HX711 pins
static const int HX711_SCK_PIN = 1;  // Clock pin
static const int HX711_DT_PIN  = 2;  // Data pin

// tb6612fng motor driver pins
// Valve Motor
static const int MOTOR_AIN1 = 8;
static const int MOTOR_AIN2 = 9;
static const int MOTOR_PWMA = 7;

// Mixer Motor
static const int MOTOR_BIN1 = 12;
static const int MOTOR_BIN2 = 11;
static const int MOTOR_PWMB = 13;

// Standby pin
static const int MOTOR_STBY = 10;

// Declare a static global instance of HX711 within the NuggetsInc namespace.
namespace NuggetsInc {
    static HX711 scale;
}

namespace NuggetsInc {

// Helper function to initialize MCPWM channels (only once)
static void initMotors() {
    static bool motorsInitialized = false;
    if (!motorsInitialized) {
        // Valve Motor MCPWM Setup on MCPWM_UNIT_0, TIMER_0, OPERATOR_A (pin MOTOR_PWMA)
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_PWMA);
        mcpwm_config_t pwm_config_valve;
        pwm_config_valve.frequency = 1000;    // 1 kHz PWM frequency
        pwm_config_valve.cmpr_a = 0;            // initial duty 0%
        pwm_config_valve.counter_mode = MCPWM_UP_COUNTER;
        pwm_config_valve.duty_mode = MCPWM_DUTY_MODE_0;
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config_valve);

        // Mixer Motor MCPWM Setup on MCPWM_UNIT_0, TIMER_1, OPERATOR_A (pin MOTOR_PWMB)
        mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR_PWMB);
        mcpwm_config_t pwm_config_mixer;
        pwm_config_mixer.frequency = 1000;    // 1 kHz PWM frequency
        pwm_config_mixer.cmpr_a = 0;            // initial duty 0%
        pwm_config_mixer.counter_mode = MCPWM_UP_COUNTER;
        pwm_config_mixer.duty_mode = MCPWM_DUTY_MODE_0;
        mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config_mixer);

        motorsInitialized = true;
    }
}

MenuState::MenuState()
    : menuIndex(0),
      previousMenuIndex(-1),
      displayUtils(new DisplayUtils()),
      calibrationFactor(0.0)  // Not calibrated yet
{
    // Initialize menu items.
    menu[0] = "Tare Scale";
    menu[1] = "Calibrate Scale";
    menu[2] = "Read Weight";
    menu[3] = "Measure Water";
    menu[4] = "Mix Water";
}

MenuState::~MenuState() {
    delete displayUtils;
}

void MenuState::onEnter() {
    // Set motor driver pins as outputs
    pinMode(MOTOR_STBY, OUTPUT);
    pinMode(MOTOR_AIN1, OUTPUT);
    pinMode(MOTOR_AIN2, OUTPUT);
    pinMode(MOTOR_BIN1, OUTPUT);
    pinMode(MOTOR_BIN2, OUTPUT);

    // Initialize the HX711 sensor using our global instance 'scale'
    scale.begin(HX711_DT_PIN, HX711_SCK_PIN);
    
    // Initialize MCPWM for the motors
    initMotors();
    
    // If we haven't calibrated yet, use a default factor.
    if (calibrationFactor == 0.0) {
        calibrationFactor = 100.0;
    }
    scale.set_scale(calibrationFactor);

    // Tare the scale (only on initial entry, if desired)
    scale.tare();

    // Initialize the display using DisplayUtils functions
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setTextColor(COLOR_WHITE);
    displayUtils->setTextSize(2);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Menu");
    displayUtils->setTextSize(1);

    for (int i = 0; i < menuItems; i++) {
        redrawLine(i, i == menuIndex);
    }
}

void MenuState::onExit() {
    displayUtils->clearDisplay();
}

void MenuState::update() {
    QueueHandle_t eventQueue = HandleEvents::getInstance().getEventQueue();
    Event event;
    
    while (xQueueReceive(eventQueue, &event, 0) == pdPASS) {
        switch (event.type) {
            case EventType::MOVE_UP:
                if (menuIndex > 0) {
                    previousMenuIndex = menuIndex;
                    menuIndex = (menuIndex - 1 + menuItems) % menuItems;
                    redrawMenuChange();
                }
                break;
            case EventType::MOVE_DOWN:
                previousMenuIndex = menuIndex;
                menuIndex = (menuIndex + 1) % menuItems;
                redrawMenuChange();
                break;
            case EventType::SELECT:
                executeSelection();
                break;
            case EventType::MOVE_LEFT:
            case EventType::MOVE_RIGHT:
                break;
            default:
                break;
        }
    }
}

void MenuState::redrawMenuChange() {
    if (previousMenuIndex != -1) {
        redrawLine(previousMenuIndex, false);
    }
    redrawLine(menuIndex, true);
}

void MenuState::redrawLine(int index, bool isSelected) {
    int y = 20 + (index * 20);
    
    if (isSelected) {
        displayUtils->fillRect(0, y - 2, displayUtils->ScreenWidth, 18, COLOR_WHITE);
        displayUtils->setTextColor(COLOR_BLACK);
        displayUtils->setTextSize(2);
    } else {
        displayUtils->fillRect(0, y - 2, displayUtils->ScreenWidth, 18, COLOR_BLACK);
        displayUtils->setTextColor(COLOR_WHITE);
        displayUtils->setTextSize(1);
    }
    
    displayUtils->setCursor(0, y);
    displayUtils->println(menu[index]);
    displayUtils->setTextColor(COLOR_WHITE);
}

void MenuState::onExitSelection() {
    // Reapply the stored calibration factor and redraw the menu.
    scale.set_scale(calibrationFactor);
    
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setTextColor(COLOR_WHITE);
    displayUtils->setTextSize(2);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Menu");
    displayUtils->setTextSize(1);
    
    for (int i = 0; i < menuItems; i++) {
        redrawLine(i, i == menuIndex);
    }
}

void MenuState::executeSelection() {
    if (menuIndex == 0) { // "Tare Scale": Zero out the load cell
        scale.tare();
        displayUtils->fillScreen(COLOR_BLACK);
        displayUtils->setCursor(0, 0);
        displayUtils->println("Scale Tared");
        delay(2000);
        onExitSelection();
    } 
    else if (menuIndex == 1) { // "Calibrate Scale": Calibrate using a known 100g weight
        scale.tare();
        displayUtils->fillScreen(COLOR_BLACK);
        displayUtils->setCursor(0, 0);
        displayUtils->println("Place 100g weight");
        displayUtils->println("and wait...");
        delay(5000);
        
        const int sampleCount = 10;
        float total = 0.0;
        for (int i = 0; i < sampleCount; i++) {
            total += scale.get_units(1);
            delay(20);
        }
        float averageReading = total / sampleCount;
        
        calibrationFactor = averageReading / 100.0;
        scale.set_scale(calibrationFactor);
        
        displayUtils->fillScreen(COLOR_BLACK);
        displayUtils->setCursor(0, 0);
        displayUtils->println("Calibrated!");
        displayUtils->println("Factor:");
        displayUtils->println(String(calibrationFactor, 4));
        delay(3000);
        onExitSelection();
    } 
    else if (menuIndex == 2) { // "Read Weight"
        displayUtils->fillScreen(COLOR_BLACK);
        
        QueueHandle_t eventQueue = HandleEvents::getInstance().getEventQueue();
        Event event;
        
        const int sampleCount = 50;
        
        while (true) {
            if (xQueueReceive(eventQueue, &event, 0) == pdPASS) {
                if (event.type == EventType::MOVE_LEFT || event.type == EventType::MOVE_RIGHT) {
                    break;
                }
            }
            
            float total = 0.0;
            for (int j = 0; j < sampleCount; j++) {
                total += scale.get_units(1);
                delay(1);
            }
            float filteredWeight = total / sampleCount;
            String message = "Weight: " + String(filteredWeight, 2) + " g";
            displayUtils->displayMessage(message);
            delay(10);
        }
        onExitSelection();
    }
    else if (menuIndex == 3) { // "Measure Water"
        measureWaterMode();
    }
    else if (menuIndex == 4) { // "Mix Water"
        mixWaterMode();
    }    
}

//
// --- Measure Water Mode ---
// After the user enters a target volume (in ml) using a "XXXX.X" format,
// this mode continuously reads the current weight (assumed 1g/ml) from the scale,
// updates the display with the current water dispensed, and controls the valve motor.
// The valve motor is always driven at 100% (via MCPWM on TIMER_0) until the target is reached.
// Pressing BACK at any time cancels the operation.
void MenuState::measureWaterMode() {
    int targetTenths = 5000;  // Default: 500.0 ml (stored as tenths)
    int selectedDigit = 0;    // Editable positions: 0 to 4
    bool inputConfirmed = false;
    
    // Display the input screen once.
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Measure Water");
    displayUtils->println("Set volume (ml):");
    char valueBuffer[10];
    int intPart = targetTenths / 10;
    int dec = targetTenths % 10;
    sprintf(valueBuffer, "%04d.%d", intPart, dec);
    displayUtils->println(valueBuffer);
    displayUtils->println("Edit digit pos:");
    displayUtils->println(String(selectedDigit));
    displayUtils->println("UP/DOWN: change digit");
    displayUtils->println("LEFT/RIGHT: move");
    displayUtils->println("SELECT: confirm");
    displayUtils->println("BACK: cancel");
    
    bool updated = false;
    while (!inputConfirmed) {
        Event event;
        if (xQueueReceive(HandleEvents::getInstance().getEventQueue(), &event, pdMS_TO_TICKS(50)) == pdPASS) {
            updated = false;
            int multiplier = 0;
            switch(selectedDigit) {
                case 0: multiplier = 10000; break;
                case 1: multiplier = 1000;  break;
                case 2: multiplier = 100;   break;
                case 3: multiplier = 10;    break;
                case 4: multiplier = 1;     break;
            }
            
            if (event.type == EventType::MOVE_UP) {
                int currentDigit = (targetTenths / multiplier) % 10;
                if (currentDigit < 9) { targetTenths += multiplier; updated = true; }
            }
            else if (event.type == EventType::MOVE_DOWN) {
                int currentDigit = (targetTenths / multiplier) % 10;
                if (currentDigit > 0) { targetTenths -= multiplier; updated = true; }
            }
            else if (event.type == EventType::MOVE_LEFT) {
                selectedDigit = (selectedDigit - 1 + 5) % 5; updated = true;
            }
            else if (event.type == EventType::MOVE_RIGHT) {
                selectedDigit = (selectedDigit + 1) % 5; updated = true;
            }
            else if (event.type == EventType::SELECT) {
                inputConfirmed = true; updated = true;
            }
            else if (event.type == EventType::BACK) {
                onExitSelection();
                return;
            }
            
            if (updated) {
                displayUtils->fillScreen(COLOR_BLACK);
                displayUtils->setCursor(0, 0);
                displayUtils->println("Measure Water");
                displayUtils->println("Set volume (ml):");
                intPart = targetTenths / 10;
                dec = targetTenths % 10;
                sprintf(valueBuffer, "%04d.%d", intPart, dec);
                displayUtils->println(valueBuffer);
                displayUtils->println("Edit digit pos:");
                displayUtils->println(String(selectedDigit));
                displayUtils->println("UP/DOWN: change digit");
                displayUtils->println("LEFT/RIGHT: move");
                displayUtils->println("SELECT: confirm");
                displayUtils->println("BACK: cancel");
            }
        }
    }
    
    float targetVolume = targetTenths / 10.0;
    
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->println("Dispensing Water:");
    displayUtils->println(String(targetVolume, 1) + " ml");
    delay(2000);
    
    // In this version, the valve motor is always on at 100% until the target is reached.
    bool finished = false;
    while (!finished) {
        // Check for BACK event to cancel.
        Event event;
        if (xQueueReceive(HandleEvents::getInstance().getEventQueue(), &event, pdMS_TO_TICKS(10)) == pdPASS) {
            if (event.type == EventType::BACK) {
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0);
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
                onExitSelection();
                return;
            }
        }
        
        // Read current water (assumed 1g/ml).
        float currentWeight = scale.get_units(1);
        
        // Refresh the "Current" line: clear a rectangle and then print the current value.
        // (Adjust the rectangle size and position as needed.)
        displayUtils->fillRect(0, 100, displayUtils->ScreenWidth, 20, COLOR_BLACK);
        displayUtils->setCursor(0, 100);
        displayUtils->println("Current: " + String(currentWeight, 1) + " ml");
        
        if (currentWeight < targetVolume) {
            // Always run the valve motor at 100% duty.
            digitalWrite(MOTOR_STBY, HIGH);
            digitalWrite(MOTOR_AIN1, HIGH);
            digitalWrite(MOTOR_AIN2, LOW);
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 100.0);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
        } 
        else {
            // Target reached (or overshot); stop the valve motor.
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, 0.0);
            mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
            finished = true;
        }
        delay(200); // Brief delay before next loop iteration.
    }
    
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->println("Final reached:");
    displayUtils->println(String(targetVolume, 1) + " ml");
    delay(3000);
    onExitSelection();
}

//
// --- Mix Water Mode ---
// In this mode the user directly changes the mixer motor speed (0–9) using UP/DOWN.
// The speed is immediately mapped to a PWM duty cycle and sent to the motor.
// The display is updated only when an input change occurs. Press BACK to exit.
void MenuState::mixWaterMode() {
    int mixerSpeedDigit = 5; // Default speed (0–9)
    
    displayUtils->fillScreen(COLOR_BLACK);
    displayUtils->setCursor(0, 0);
    displayUtils->println("Mix Water");
    displayUtils->println("Speed: " + String(mixerSpeedDigit));
    
    digitalWrite(MOTOR_STBY, HIGH);
    digitalWrite(MOTOR_BIN1, HIGH);
    digitalWrite(MOTOR_BIN2, LOW);
    
    int pwmValue = map(mixerSpeedDigit, 0, 9, 0, 255);
    float duty = (pwmValue / 255.0) * 100.0;
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, duty);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    
    bool running = true;
    while (running) {
        Event event;
        if (xQueueReceive(HandleEvents::getInstance().getEventQueue(), &event, pdMS_TO_TICKS(50)) == pdPASS) {
            bool updated = false;
            if (event.type == EventType::MOVE_UP) {
                if (mixerSpeedDigit < 9) { mixerSpeedDigit++; updated = true; }
            } 
            else if (event.type == EventType::MOVE_DOWN) {
                if (mixerSpeedDigit > 0) { mixerSpeedDigit--; updated = true; }
            } 
            else if (event.type == EventType::BACK) {
                running = false;
            }
            
            if (updated) {
                pwmValue = map(mixerSpeedDigit, 0, 9, 0, 255);
                duty = (pwmValue / 255.0) * 100.0;
                mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, duty);
                mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
                displayUtils->fillScreen(COLOR_BLACK);
                displayUtils->setCursor(0, 0);
                displayUtils->println("Mix Water");
                displayUtils->println("Speed: " + String(mixerSpeedDigit));
            }
        }
    }
    
    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, 0.0);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
    digitalWrite(MOTOR_BIN1, LOW);
    digitalWrite(MOTOR_BIN2, LOW);
    onExitSelection();
}

} // namespace NuggetsInc
