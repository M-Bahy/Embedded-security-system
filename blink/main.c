#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"  
#include "pico/stdlib.h"
#include <stdarg.h>



// #define RS_PIN 2
// #define EN_PIN 4
// #define D4_PIN 5
// #define D5_PIN 6
// #define D6_PIN 7
// #define D7_PIN 8




//////////////////////////////////// LCD FUNCTIONS ////////////////////////////////////

// Define GPIO pins for LCD interface
#define RS_PIN 2
#define E_PIN 4
#define D4_PIN 5
#define D5_PIN 6
#define D6_PIN 7
#define D7_PIN 8

// Function to send a nibble to the LCD
void lcd_send_nibble(uint8_t data) {
    gpio_put(D4_PIN, data & 1);
    gpio_put(D5_PIN, (data >> 1) & 1);
    gpio_put(D6_PIN, (data >> 2) & 1);
    gpio_put(D7_PIN, (data >> 3) & 1);
    gpio_put(E_PIN, 1);
    sleep_ms(1);
    gpio_put(E_PIN, 0);
    sleep_ms(1);
}


// Function to send a command to the LCD
void lcd_send_command(uint8_t command) {
    gpio_put(RS_PIN, 0);
    lcd_send_nibble(command >> 4);
    lcd_send_nibble(command & 0x0F);
}

// Function to send data to the LCD
void lcd_send_data(uint8_t data) {
    gpio_put(RS_PIN, 1);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_second_line() {
    lcd_send_command(0xC0); // 0xC0 is the DDRAM address of the second line's first position
}

void lcd_first_line() {
    lcd_send_command(0x80); // 0x80 is the DDRAM address of the first line's first position
}

// Function to initialize the LCD
void lcd_init() {
    // Set GPIO pins to output
    gpio_init(RS_PIN);
    gpio_init(E_PIN);
    gpio_init(D4_PIN);
    gpio_init(D5_PIN);
    gpio_init(D6_PIN);
    gpio_init(D7_PIN);
    gpio_set_dir(RS_PIN, GPIO_OUT);
    gpio_set_dir(E_PIN, GPIO_OUT);
    gpio_set_dir(D4_PIN, GPIO_OUT);
    gpio_set_dir(D5_PIN, GPIO_OUT);
    gpio_set_dir(D6_PIN, GPIO_OUT);
    gpio_set_dir(D7_PIN, GPIO_OUT);

    // Initialize LCD sequence
    sleep_ms(15); // Wait for more than 15 ms after VCC rises to 4.5V
    lcd_send_command(0x03); // Function set (Interface is 8 bits long.)
    sleep_ms(5); // Wait for more than 4.1 ms
    lcd_send_command(0x03); // Function set (Interface is 8 bits long.)
    sleep_us(100); // Wait for more than 100 μs
    lcd_send_command(0x03); // Function set (Interface is 8 bits long.)
    lcd_send_command(0x02); // Function set (Interface is 4 bits long.)

    lcd_send_command(0x28); // Function set: 4-bit/2-line
    lcd_send_command(0x0C); // Display ON/OFF control: Display ON, Cursor OFF, Blink OFF
    lcd_send_command(0x01); // Clear display
    sleep_ms(2);
    lcd_send_command(0x06); // Entry mode set: Increment cursor, No display shift
}

// Function to write a string to the LCD
void lcd_write_string(const char *str) {
    while (*str) {
        lcd_send_data((uint8_t)(*str));
        str++;
    }
    sleep_ms(10);
}

int abs(int x) {
    return x >= 0 ? x : -1*x;
}

void lcd_write_formatted_string(const char *format, ...) {
    char buffer[32]; // Ensure the buffer is large enough for your string
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Now write the buffer to the LCD
    for (int i = 0; buffer[i] != '\0'; i++) {
        lcd_send_data(buffer[i]);
    }
}

//////////////////////////////////// END LCD FUNCTIONS ////////////////////////////////////








#define GPIO_PIN 6
#define PWM_PIN_OUT 6
#define PWM_FREQ 1


float clockDiv = 64;
float wrap = 39062;
bool direction = true;
int currentMillis = 400;
int servoPin = 0;


void setMillis(int servoPin, float millis)
{
    pwm_set_gpio_level(servoPin, (millis/20000.f)*wrap);
}

void setServo(int servoPin, float startMillis)
{
    gpio_set_function(servoPin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    
    uint64_t clockspeed = clock_get_hz(5);
    clockDiv = 64;
    wrap = 39062;

    while (clockspeed/clockDiv/50 > 65535 && clockDiv < 256) clockDiv += 64; 
    wrap = clockspeed/clockDiv/50;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);

    pwm_init(slice_num, &config, true);

    setMillis(servoPin, startMillis);
}


// void lcd_send_nibble(uint8_t nibble) {
//     gpio_put(D4_PIN, (nibble >> 0) & 1);
//     gpio_put(D5_PIN, (nibble >> 1) & 1);
//     gpio_put(D6_PIN, (nibble >> 2) & 1);
//     gpio_put(D7_PIN, (nibble >> 3) & 1);

//     gpio_put(EN_PIN, 1);
//     sleep_us(1);
//     gpio_put(EN_PIN, 0);
//     sleep_us(100);
// }

// void lcd_send_byte(uint8_t data) {
//     lcd_send_nibble(data >> 4);  // Send high nibble
//     lcd_send_nibble(data & 0x0F);  // Send low nibble
// }

// void lcd_send_cmd(uint8_t cmd) {
//     gpio_put(RS_PIN, 0);
//     lcd_send_byte(cmd);
// }

// void lcd_send_char(char data) {
//     gpio_put(RS_PIN, 1);
//     lcd_send_byte(data);
// }

// void lcd_init() {
//     gpio_init(RS_PIN);
//     gpio_init(EN_PIN);
//     gpio_init(D4_PIN);
//     gpio_init(D5_PIN);
//     gpio_init(D6_PIN);
//     gpio_init(D7_PIN);

//     gpio_set_dir(RS_PIN, GPIO_OUT);
//     gpio_set_dir(EN_PIN, GPIO_OUT);
//     gpio_set_dir(D4_PIN, GPIO_OUT);
//     gpio_set_dir(D5_PIN, GPIO_OUT);
//     gpio_set_dir(D6_PIN, GPIO_OUT);
//     gpio_set_dir(D7_PIN, GPIO_OUT);

//     // Initialization sequence
//     sleep_ms(20);
//     lcd_send_nibble(0x03);
//     sleep_ms(5);
//     lcd_send_nibble(0x03);
//     sleep_us(100);
//     lcd_send_nibble(0x03);

//     lcd_send_nibble(0x02);
//     sleep_us(100);

//     lcd_send_cmd(0x28);
//     lcd_send_cmd(0x0C);
//     lcd_send_cmd(0x06);
//     lcd_send_cmd(0x01);
//     sleep_us(100);
// }

// void lcd_set_cursor(uint8_t row, uint8_t col) {
//     uint8_t cmd = 0x80 + (row * 0x40) + col;
//     lcd_send_cmd(cmd);
// }

// void lcd_print(const char* str) {
//     while (*str) {
//         lcd_send_char(*str);
//         str++;
//     }
// }

int flag=0;
int rotate = 0;
int main() {
  
    stdio_init_all();

    lcd_init();
    const uint sensor_Pir = 15; 
  
  //  lcd_set_cursor(0, 0);
  
    //lcd_write_string("bahy");
 const uint buzzer  = 1;
 gpio_init(buzzer);
    gpio_set_dir(buzzer, GPIO_OUT);
    gpio_set_dir(sensor_Pir, GPIO_IN);
    //gpio_init(pulp_pin);
    //gpio_set_dir(pulp_pin, GPIO_OUT);
    char recieved_char='0';

   setServo(servoPin, currentMillis);
    // lcd_init();
    // lcd_set_cursor(0, 0);
    // lcd_print("bbbbb motion");
 while(true){
    int pir_value=gpio_get(sensor_Pir);
    if(pir_value==1){
         lcd_send_command(0x01);
  lcd_second_line();
    lcd_write_string("Say the password");
  while(true){
   
    //sleep_ms()
   recieved_char = getchar_timeout_us(50); 
    if(recieved_char=='1'){
      //  setMillis(servoPin,1000);
    lcd_send_command(0x01);
  lcd_second_line();
    lcd_write_string("Opening door");





 while (rotate==0)
    {
        if(rotate==0){
                
        currentMillis += (direction)?5:-5;
     if (currentMillis >= 2400) direction = false;
        if (currentMillis <= 400) direction = true;
       // direction = true;
        setMillis(servoPin, currentMillis);
       
        sleep_ms(10);
        if(currentMillis==400)
            rotate= 1;

        }
    
    }








        break;
        }
     if(recieved_char=='2'){
        lcd_send_command(0x01);
  lcd_second_line();
    lcd_write_string("haramy");
         gpio_put(buzzer,1);
         sleep_ms(2500);
        break;
        //
        }    
   printf("say open ");
  }
    }
    }

 }
 // Your main code goes here
/*
    if (cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
        printf("failed to initialise\n");
        return 1;
    }
    printf("initialised\n");

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
        printf("failed to connect\n");
        return 1;
    }

    printf("connected\n");
    /*
     const uint red = 3; 
     const uint sens=2;
     stdio_init_all();
     gpio_init(red);
     gpio_init(sens);
     gpio_set_dir(sens, GPIO_IN);
     gpio_set_dir(red, GPIO_OUT);
     while (true) {
        int val = gpio_get(sens);
        printf("Value: %d\n", val);
        if (val != 1) {
            printf("ZEROOOOO\n");
        }
       if( gpio_get(sens)){
         gpio_put(red,true);
       }
       else{
        gpio_put(red,false);
      // sleep_ms(100);
       }
       
        
    }
    */

   /*
    ///pir init
    stdio_init_all();
    const uint sens=6;
    gpio_init(sens);
    gpio_set_dir(sens, GPIO_IN);
    lcd_init();
    lcd_set_cursor(0, 0);
    while(true){
         int val = gpio_get(sens);
         if(val==1){
            //lcd on and mic on
            printf("nour");
             lcd_print("bbbbb Say The Password");

         }
         else{
            printf("no");
         }

    }
    
    */
    /*
    stdio_init_all();
    lcd_init();

    lcd_set_cursor(0, 0);
    lcd_print("bbbbb Say The Password");
    while (true) {
        tight_loop_contents();
    }
*/

 /*
   //stdio_init_all();
    setServo(servoPin, currentMillis);
    while (true)
    {
        if(rotate==0){
                
        currentMillis += (direction)?5:-5;
     if (currentMillis >= 2400) direction = false;
        if (currentMillis <= 400) direction = true;
       // direction = true;
        setMillis(servoPin, currentMillis);
       
        sleep_ms(10);
        if(currentMillis==400)
            rotate= 1;

        }
    
    }
}
  */

