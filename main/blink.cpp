
// .\bin\openocd -l out.txt -d3 -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp32-wrover.cfg

/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
//#include "HardwareSerial.h"

#include "arduino.h"

/*SPI Includes*/
#include "driver/spi_master.h"
#include "iot_lcd.h"
#include "Adafruit_GFX.h"
#include "image.h"
#include "FreeSans9pt7b.h"
static CEspLcd* lcd_obj = NULL;

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
//#define BLINK_GPIO CONFIG_BLINK_GPIO
#define BLINK_GPIO CONFIG_BLINK_GPIO

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction((gpio_num_t)BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level((gpio_num_t)BLINK_GPIO, 1);
        vTaskDelay(1100 / portTICK_PERIOD_MS);
    }
}

extern "C" void esp_draw()
{
    /*Initilize ESP32 to scan for Access points*/
    //nvs_flash_init();
    /*
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
     */
    /*Initialize LCD*/
    lcd_conf_t lcd_pins = {
        .lcd_model    = LCD_MOD_AUTO_DET,
        .pin_num_miso = GPIO_NUM_25,
        .pin_num_mosi = GPIO_NUM_23,
        .pin_num_clk  = GPIO_NUM_19,
        .pin_num_cs   = GPIO_NUM_22,
        .pin_num_dc   = GPIO_NUM_21,
        .pin_num_rst  = GPIO_NUM_18,
        .pin_num_bckl = GPIO_NUM_5,
        .clk_freq     = 40 * 1000 * 1000,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .spi_host = HSPI_HOST,
        .init_spi_bus = true,
    };

    if (lcd_obj == NULL) {
        lcd_obj = new CEspLcd(&lcd_pins);
    }
    printf("lcd id: 0x%08x\n", lcd_obj->id.id);

    /*Welcome screen*/
    int x = 0, y = 0;
    int dim = 6;
    uint16_t rand_color;
    lcd_obj->setRotation(3);
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < 10 - 2 * i; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(20000);
            x++;
        }
        x--;
        for (int j = 0; j < 10 - 2 * i; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(20000);
            y++;
        }
        y--;
        for (int j = 0; j < 10 - 2 * i - 1; j++) {
            rand_color = rand();
            lcd_obj->fillRect(x * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(20000);
            x--;
        }
        x++;
        for (int j = 0; j < 10 - 2 * i - 1; j++) {
            rand_color = rand();
            lcd_obj->fillRect((x - 1) * 32, y * 24, 32, 24, rand_color);
            ets_delay_us(20000);
            y--;
        }
        y++;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    /*ESPecifications*/
    lcd_obj->setRotation(2);
    lcd_obj->fillScreen(COLOR_ESP_BKGD);
    lcd_obj->setTextSize(1);
    lcd_obj->drawBitmap(0, 0, esp_logo, 137, 26);

    lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("CPU",                                     3, 40);
    lcd_obj->setFont(NULL);
    lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
    lcd_obj->drawString("Xtensa Dual-Core 32-bit LX6 MPU",         3, 50);
    lcd_obj->drawString("Max Clock Speed at 240 MHz & 600 DMIPS ", 3, 60);
    lcd_obj->drawString("at up to 600 DMIPS",                      3, 70);
    lcd_obj->drawString("Memory: 520 KiB SRAM",                    3, 80);

    lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("Wireless connectivity",               3,  110);
    lcd_obj->setFont(NULL);
    lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
    lcd_obj->drawString("Wi-Fi: 802.11 b/g/n/e/i",            3,  120);
    lcd_obj->drawString("Bluetooth: v4.2 BR/EDR and BLE",      3,  130);

    lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("Power Management",                     3, 160);
    lcd_obj->setFont(NULL);
    lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
    lcd_obj->drawString("Internal LDO",                         3, 170);
    lcd_obj->drawString("Individual power domain for RTC",      3, 180);
    lcd_obj->drawString("5uA deep sleep current",               3, 190);
    lcd_obj->drawString("Wake up from GPIO interrupt" ,         3, 200);
    lcd_obj->drawString("Wake up from timer, ADC measurements", 3, 210);
    lcd_obj->drawString("Wake up from capacitive sensor intr",  3, 220);

    lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("Security",                               3, 250);
    lcd_obj->setFont(NULL);
    lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
    lcd_obj->drawString("IEEE 802.11 standard security features", 3, 260);
    lcd_obj->drawString("Secure boot & Flash Encryption",         3, 270);
    lcd_obj->drawString("Cryptographic Hardware Acceleration",    3, 280);
    lcd_obj->drawString("AES, RSA, SHA-2, EEC, RNG",              3, 290);
    lcd_obj->drawString("1024-bit OTP",                           3, 300);

    vTaskDelay(4000 / portTICK_PERIOD_MS);
    lcd_obj->fillRect(0, 28, 240, 320, COLOR_ESP_BKGD);

    lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("Peripheral Interfaces",               3, 40);
    lcd_obj->setFont(NULL);
    lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
    lcd_obj->drawString("12-bit DAC, 18 channels",             3, 50);
    lcd_obj->drawString("8-bit  DAC,  2 channels",             3, 60);
    lcd_obj->drawString("SPI,  4 channels",                   3, 70);
    lcd_obj->drawString("I2S,  4 channels",                   3, 80);
    lcd_obj->drawString("I2C,  2 channels",                   3, 90);
    lcd_obj->drawString("UART, 3 channels",                   3, 100);
    lcd_obj->drawString("SD/SDIO/MMC Host",                   3, 110);
    lcd_obj->drawString("SDIO/SPI Slave",                     3, 120);
    lcd_obj->drawString("Ethernet MAC with DMA & IEEE 1588",   3, 130);
    lcd_obj->drawString("CAN bus 2.0",                        3, 140);
    lcd_obj->drawString("IR/RMT (Tx/Rx)",                     3, 150);
    lcd_obj->drawString("Motor PWM",                              3, 160);
    lcd_obj->drawString("LED PWM, 16 channels",               3, 170);
    lcd_obj->drawString("Ultra Low Power Analog Pre-Amp",      3, 180);
    lcd_obj->drawString("Hall Effect Sensor",                 3, 190);
    lcd_obj->drawString("Capacitive Touch Sense, 10 channels", 3, 200);
    lcd_obj->drawString("Temperature Sensor",                  3, 210);
    vTaskDelay(4000 / portTICK_PERIOD_MS);

    lcd_obj->fillScreen(COLOR_ESP_BKGD);
    lcd_obj->drawBitmap(0, 0, esp_logo, 137, 26);
    lcd_obj->drawRoundRect(0, 0, 240, 320, 3, COLOR_WHITE);
    lcd_obj->drawFastHLine(0, 25, 320, COLOR_WHITE);
    lcd_obj->setTextColor(COLOR_WHITE, COLOR_ESP_BKGD);
    lcd_obj->drawString("Wifi-scan", 180, 10);
    lcd_obj->setFont(&FreeSans9pt7b);
    lcd_obj->drawString("AP Name",    10, 50);
    lcd_obj->drawString("RSSI",      180, 50);
    lcd_obj->setFont(NULL);

    //ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    delay(1000);
    uint16_t ap_num = 20;
    //wifi_ap_record_t ap_records[20];
    //ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));
    delay(1000);
    printf("Found %d access points:\n", ap_num);

    /*Print 10 of them on the screen*/
    for (uint8_t i = 0; i < ap_num; i++) {
        lcd_obj->drawNumber(i + 1, 10, 60 + (i * 10));
        lcd_obj->setTextColor(COLOR_YELLOW, COLOR_ESP_BKGD);
        //lcd_obj->drawString((char *) ap_records[i].ssid, 30, 60 + (i * 10));
        lcd_obj->drawString("SSID", 30, 60 + (i * 10));
        lcd_obj->setTextColor(COLOR_GREEN, COLOR_ESP_BKGD);
        //lcd_obj->drawNumber(100 + ap_records[i].rssi, 200, 60 + (i * 10));
        lcd_obj->drawNumber(77, 200, 60 + (i * 10));
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}

extern "C" {
void app_main();
}
void app_main() {
	esp_draw();
    //xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
