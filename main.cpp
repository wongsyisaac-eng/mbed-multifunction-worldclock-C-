#include "mbed.h"
#include "C12832.h"
#include <string>


class Potentiometer                                 //Begin Potentiometer class definition
{
private:                                            //Private data member declaration
    AnalogIn inputSignal;                           //Declaration of AnalogIn object
    float VDD, currentSampleNorm, currentSampleVolts; //Float variables to speficy the value of VDD and most recent samples

public:                                             // Public declarations
    Potentiometer(PinName pin, float v) : inputSignal(pin), VDD(v) {}   //Constructor - user provided pin name assigned to AnalogIn...
                                                                        //VDD is also provided to determine maximum measurable voltage
    float amplitudeVolts(void)                      //Public member function to measure the amplitude in volts
    {
        return (inputSignal.read()*VDD);            //Scales the 0.0-1.0 value by VDD to read the input in volts
    }
    
    float amplitudeNorm(void)                       //Public member function to measure the normalised amplitude
    {
        return inputSignal.read();                  //Returns the ADC value normalised to range 0.0 - 1.0
    }
    
    void sample(void)                               //Public member function to sample an analogue voltage
    {
        currentSampleNorm = inputSignal.read();       //Stores the current ADC value to the class's data member for normalised values (0.0 - 1.0)
        currentSampleVolts = currentSampleNorm * VDD; //Converts the normalised value to the equivalent voltage (0.0 - 3.3 V) and stores this information
    }
    
    float getCurrentSampleVolts(void)               //Public member function to return the most recent sample from the potentiometer (in volts)
    {
        return currentSampleVolts;                  //Return the contents of the data member currentSampleVolts
    }
    
    float getCurrentSampleNorm(void)                //Public member function to return the most recent sample from the potentiometer (normalised)
    {
        return currentSampleNorm;                   //Return the contents of the data member currentSampleNorm  
    }

};

class SamplingPotentiometer : public Potentiometer{
    private:
    float samplingFrequency, samplingPeriod, latestSample;
    Ticker sampler;

    public:
    SamplingPotentiometer(PinName p, float v, float fs) : Potentiometer(p, v){
        samplingFrequency = fs;
        samplingPeriod = 1.0 / fs;

        sampler.attach(callback(this, &SamplingPotentiometer::updateLatestSample), samplingPeriod);

        latestSample = 0.0;
    }

    void updateLatestSample() {
        sample(); // Call the sample method from Potentiometer
        latestSample = getCurrentSampleNorm(); // Assign the normalized current sample to latestSample
    }

    float getLatestSample() {
        return latestSample;
    }

    int getTimeZoneIndex(int totalZones) {
        updateLatestSample();
        float potentiometerValue = getLatestSample();
        return static_cast<int>(potentiometerValue * (totalZones - 1));
    }
};

class Clock {
    private:
    int time_hour, time_min, time_secs;
    bool running, countingDown;
    int totalSecondsForCountdown;
    
    public:
    Clock(bool startRunning) : time_hour(0), time_min(0), time_secs(0), running(startRunning){}
    void reset(){
        time_hour = 0;
        time_min = 0;
        time_secs = 0;
    }
    void startClock(){
        running = true;
    };
    void stopClock(){
        running = false;
    }
    void startCountdown(){
        countingDown = true;
    };
    void stopCountdown(){
        countingDown = false;
    };
    void tick (){
        if(running){
            time_secs++;
            if(time_secs == 60) {
                time_secs = 0;
                time_min++;
                if(time_min == 60){
                    time_min = 0;
                    time_hour++;
                    if(time_hour == 24){
                        time_hour = 0;
                    }
                }
            }
        }
    }
    void detick (){
        if(countingDown){
            time_secs--;
            if(time_secs == -1) {
                time_secs = 59;
                time_min--;
                if(time_min == -1){
                    time_min = 59;
                    time_hour--;
                    if(time_hour == -1){
                        time_hour = 0;
                    }
                }
            }
        }
    }
    void setClock(int newHour, int newMin, int newSecs){
        time_hour = newHour;
        time_min = newMin;
        time_secs = newSecs;
    }
    int getHours(){
        return time_hour;
    }
    int getMins(){
        return time_min;
    }
    int getSecs(){
        return time_secs;
    }
    void setTotalSecondsForCountdown(int seconds) {
        totalSecondsForCountdown = seconds;
    }

    int getTotalSecondsForCountdown() {
        return totalSecondsForCountdown;
    }
};

struct TimeZone {
    std::string cityName;
    float offsetHours;      // Offset from GMT in hours
};



class WorldClock {
private:
    TimeZone timeZones[25]; // Your time zones array
    int timeZoneIndex;

public:
    WorldClock() {
    TimeZone timeZones[25] = {
        {"Alofi", -11}, {"Honolulu", -10}, {"Anchorage", -9}, {"San Jose", -8}, {"Denver", -7}, 
        {"Mexico City", -6}, {"Detroit", -5}, {"Bahamas", -4}, {"Buenos Aires", -3}, {"Grytviken", -2}, 
        {"Lagoa", -1}, {"Manchester", 0}, {"Amsterdam", 1}, {"Cairo", 2}, {"Baghdad", 3}, 
        {"Dubai", 4}, {"Kabul", 5}, {"Dhaka", 6}, {"Yangon", 6.5}, {"Bangkok", 7}, 
        {"Hong Kong", 8}, {"Seoul", 9}, {"Melbourne", 10}, {"Micronesia", 11}, {"Auckland", 12}
        };
    }

    void updateIndex(int index) {
        timeZoneIndex = index;
    }
};


class Speaker {                         // Begin Speaker class definition
private:                                // Private member declaration
    DigitalOut outputSignal;            // Declaration of DigitalOut object
    bool state;                         // Can be set to either 1 or 0 to record output value   

public:                                 // Public declaration
    Speaker(PinName pin) : outputSignal(pin) {  // Constructor, user chosen PinName assigned to DigitalOut
        state = 0;                              // Sets state value to false;
    }

    void on(void) {
        outputSignal = 1;               // Sets output to 1 (high)
        state = 1;                    // Sets state value to true;    
    }

    void off(void) {
        outputSignal = 0;               // Sets output to 0 (low)
        state = 0;                      // Sets state value to false;
    }

    void toggle(void) {
        outputSignal = !state;          // Toggle output signal
        state = !state;                 // Toggle state
    }

    bool getStatus(void){
        return state;
    }
};

Speaker speaker(D6);

class Buzzer{
    private:
        Ticker buzzer; 
    public:
    void startBuzzing(float timePeriod) {
        buzzer.attach(callback(&speaker, &Speaker::toggle), timePeriod);
    }
    void stopBuzzing() {
        buzzer.detach();    
    }       
};

class LED {                                 // RGB LED class                                     
private:
    DigitalOut red, green, blue;                                                             
    char state;
public:
    LED(PinName r, PinName g, PinName b) : red(r), green(g), blue(b){  
        Off();                                                  
    }                                                                  

    void Red(int r){
        red = !r;
        green = r;
        blue = r;
    }

    void Green(int g){
        red = g;                                    // active low so on = 0, hence why !g
        green = !g;                     
        blue = g;
    }

    void Blue(int b){
        red = b;
        green = b;
        blue = !b;
    }

    void Yellow(int y){
        red = !y;
        green = !y;
        blue = y;
    }

    void setColour(int r, int g, int b) {   
        red = !r;                           
        green = !g;                        
        blue = !b;                          
        state = 1;                          
    }

    void Off(void) {                       
        red = 1;                            
        green = 1;                          
        blue = 1;                           
        state = 0;                          
    }

};

C12832 lcd(D11, D13, D12, D7, D10);                     // Initialise c12832 object, LCD,  with corresponding pins
LED led(D5, D9, D8);                                    // Initialise LED object, led, with corresponding pins

typedef enum {init, setTime, currentTime, worldTime, stopwatch, countdown, countdownComplete} ProgramState;    //Definition of the enum that refers to the states of the program
ProgramState state;

typedef enum{idle, running, last} StopwatchState;
StopwatchState stopwatchState;

typedef enum{set, counting, complete} CountdownState;
CountdownState countdownState;

Clock Stopwatch(false);                 // False - do not tick by default
Clock GlobalClock(true);                // True - tick by default
Clock countdownClock(false);

void clearScreen(){
    wait(0.25);
    lcd.locate(0,0);
    lcd.cls();
};

void tickFunction(){
    Stopwatch.tick();
    GlobalClock.tick();

    static bool greenLED = 0;
    if (countdownState == counting) {
        countdownClock.detick();
        greenLED = !greenLED;
        led.setColour(0,greenLED,0);
        // Check if countdown has reached 0
        if (countdownClock.getMins() == 0 && countdownClock.getSecs() == 0) {
            countdownState = complete;
            countdownClock.stopCountdown();
            // display countdownComplete state irrespective of current screen
            state = countdownComplete;
            led.setColour(0,1,0);
        }
    }
};

void displayInit(){
    clearScreen();
    lcd.locate(0,0);
    lcd.printf("Press Fire to set time");
    lcd.locate(0,8);
    lcd.printf("%02d:%02d:%02d", GlobalClock.getHours(), GlobalClock.getMins(), GlobalClock.getSecs());

};

void displaySetTime(){
    clearScreen();
    lcd.locate(0,0);
    lcd.printf("Set new time (HH:MM)"); 
    lcd.locate(0,8);
    lcd.printf("%02d:%02d", GlobalClock.getHours(), GlobalClock.getMins());
};

void displayCurrentTime(){
    clearScreen();
    lcd.locate(0,0);
    lcd.printf("Current Time: "); 
    lcd.printf("%02d:%02d:%02d", GlobalClock.getHours(), GlobalClock.getMins(), GlobalClock.getSecs());
    lcd.locate(0,8);
    lcd.printf("                                            "); 
};


void displayWorldTime(SamplingPotentiometer& potentiometer, Clock& globalClock, TimeZone* timeZones, int totalZones) {
    int timeZoneIndex = potentiometer.getTimeZoneIndex(totalZones);
    
    int offsetHours = static_cast<int>(timeZones[timeZoneIndex].offsetHours);
    int offsetMinutes = static_cast<int>((timeZones[timeZoneIndex].offsetHours - offsetHours) * 60); // Fractional part converted to minutes
    
    int hours = globalClock.getHours() + offsetHours; 
    int minutes = globalClock.getMins() + offsetMinutes; 

    // Handle overflow and underflow of hours
    if (hours >= 24) {
        hours -= 24;
    } else if (hours < 0) {
        hours += 24;
    }

    if (minutes >= 60) {
        minutes -= 60;
        hours++;
    } else if (minutes < 0) {
        minutes += 60;
        hours--;
    }

    // Clear the screen and display the time and city name
    lcd.locate(0,0);
    lcd.printf("                                 ");
    lcd.locate(0,0);
    lcd.printf("%s   (GMT %+02.2f)", timeZones[timeZoneIndex].cityName.c_str(), timeZones[timeZoneIndex].offsetHours);
    lcd.locate(0,10);
    lcd.printf("                                     ");
    lcd.locate(0,10);
    lcd.printf("%02d:%02d:%02d", hours, minutes, GlobalClock.getSecs());
    lcd.locate(0,20);
    lcd.printf("                                     ");
    lcd.locate(0,20);
    lcd.printf("%02d:%02d:%02d (Manchester)", GlobalClock.getHours(), GlobalClock.getMins(), GlobalClock.getSecs());
}

void displayStopwatch(){
    clearScreen();
    lcd.locate(0,0);
    switch(stopwatchState) {
        case idle:
            led.setColour(0, 0, 1);
            lcd.locate(0,0);
            lcd.printf("Stopwatch: Inactive"); 
            lcd.locate(0,8);
            lcd.printf("Time: 00:00"); 
            break;
        case running:
            led.setColour(0, 0, 1);
            lcd.locate(0,0);
            lcd.printf("Stopwatch: Active"); 
            lcd.locate(0,8);
            lcd.printf("Elapsed Time: %02d:%02d", Stopwatch.getMins(), Stopwatch.getSecs());
            break;
        case last:
            led.setColour(0, 0, 1);
            lcd.locate(0,0);
            lcd.printf("Stopwatch: Inactive"); 
            lcd.locate(0,8);
            lcd.printf("Last Time: %02d:%02d", Stopwatch.getMins(), Stopwatch.getSecs());
            break;
        default:
            stopwatchState = idle;         
    }
};

void displayCountdown() {
    clearScreen();
    switch(countdownState){
        case set:
            lcd.locate(0,0);
            lcd.printf("Set Countdown Period:  "); 
            lcd.locate(0,8);
            lcd.printf("%02d:%02d", countdownClock.getMins(), countdownClock.getSecs());
            break;
        case counting:
            lcd.locate(0,0);
            lcd.printf("Countdown Timer: Running "); 
            lcd.locate(0,8);
            int totalSeconds = countdownClock.getTotalSecondsForCountdown();
            int remainingSeconds = countdownClock.getMins() * 60 + countdownClock.getSecs();
            lcd.printf("%d  /  %d", remainingSeconds, totalSeconds);
            break;
        case complete:
            state = countdownComplete;
            break;
    }
};

void displayCompleteScreen(){
    clearScreen();
    lcd.locate(0,0);
    lcd.printf("Countdown has finished!");
};

void upISR(){
    clearScreen();
    led.Off();
    switch (state){
            case (init):
                state = countdown;
                break;
            case (currentTime):
                state = init;
                break;
            case (worldTime):
                state = currentTime;
                break;
            case (stopwatch):
                state = worldTime;
                break;
            case (countdown):
                state = stopwatch;
                break;
    }
};

void downISR(){
    clearScreen();
    led.Off();
    switch (state){
        case(init):
            state = currentTime;
            break;
        case (currentTime):
            state = worldTime;
            break;
        case (worldTime):
            state = stopwatch;
            break;
        case (stopwatch):
            state = countdown;
            break;
        case (countdown):
            state = init;
            break;
    }
};

void fireISR(){
    clearScreen();
    led.Off();
    switch (state){
        case(init):
            state = setTime;
            break;
        case(setTime):
            state = init;
            break;
        case(stopwatch):
            switch(stopwatchState){
                case(idle):
                    stopwatchState = running;
                    Stopwatch.startClock();
                    break;
                case(running):
                    stopwatchState = last;
                    Stopwatch.stopClock();
                    break;
                case(last):
                    stopwatchState = running;
                    Stopwatch.reset();
                    Stopwatch.startClock();
                    break;
            }
            break;
        case(countdown):
            switch(countdownState){
                case(set):
                    countdownState = counting;
                    countdownClock.startCountdown();
                    break;
                case(counting):
                    countdownState = counting;
                    break;
                case(complete):
                    countdownState = set;
                    countdownClock.stopCountdown();
                    countdownClock.reset();
                    break;
                default:
                    countdownState = set;
            }
            break;
        case(countdownComplete):
            state = countdown;
            countdownState = set;
            countdownClock.stopCountdown();
            countdownClock.reset();
            break;
    }
};

// Function to update the t7  jime based on the potentiometer value
void updateTimeFromPotentiometer() {
    SamplingPotentiometer leftHand(A0, 3.3, 100);
    SamplingPotentiometer rightHand(A1, 3.3, 100);
    leftHand.updateLatestSample();
    rightHand.updateLatestSample();
    // Read the latest potentiometer value (scaled to 0-1)
    float potentiometerValueH = leftHand.getLatestSample();
    float potentiometerValueM = rightHand.getLatestSample();
    // Map the potentiometer value to hours (0-23) and minutes (0-59)
    int hours = static_cast<int>(potentiometerValueH * 24); // Scale to 0-23
    int minutes = static_cast<int>(potentiometerValueM * 60); // Scale to 0-59

    
    // Set the time in the GlobalClock object
    GlobalClock.setClock(hours, minutes, 0);
};

void updateCountdownFromPotentiometer() {
    SamplingPotentiometer leftHand(A0, 3.3, 100);
    SamplingPotentiometer rightHand(A1, 3.3, 100);
    leftHand.updateLatestSample();
    rightHand.updateLatestSample();
    // Read the latest potentiometer value (scaled to 0-1)
    float potentiometerValueM = leftHand.getLatestSample();
    float potentiometerValueS = rightHand.getLatestSample();
    // Map the potentiometer value to hours (0-23) and minutes (0-59)
    int minutes = static_cast<int>(potentiometerValueM * 60); // Scale to 0-23
    int seconds = static_cast<int>(potentiometerValueS * 60); // Scale to 0-59

    int totalSeconds = minutes * 60 + seconds;
    countdownClock.setTotalSecondsForCountdown(totalSeconds);

    
    // Set the time in the GlobalClock object
    countdownClock.setClock(0, minutes, seconds);
};

void updateTimeZoneFromPotentiometer(){
    SamplingPotentiometer leftHand(A0, 3.3, 100);
    leftHand.updateLatestSample();
    float potentiometerValue = leftHand.getLatestSample();
    //timeZoneIndex = static_cast<int>(potentiometerValue * 26);
};


int main(){
    Ticker ticker;
    ticker.attach(&tickFunction, 1.0);

    InterruptIn up(A2);                     // Create digital in attached to up pin of joystick
    up.rise(&upISR);                        // Attach rising edge interrupt to the up switch, with address of up ISR as paremeter
    InterruptIn down(A3);                   // Create digital in attached to down pin of joystick
    down.rise(&downISR);                    // Attach rising edge interrupt to the up switch, with address of down ISR as paremeter
    InterruptIn fire(D4);                 // Create digital in attached to centre pin of joystick
    fire.rise(&fireISR);                // Attach rising edge interrupt to the up switch, with address of centre ISR as paremeter
    
    WorldClock worldClock;
    SamplingPotentiometer leftHand(A0, 3.3, 100);
        
    TimeZone timeZones[25] = {
        {"Alofi", -11}, {"Honolulu", -10}, {"Anchorage", -9}, {"San Jose", -8}, {"Denver", -7}, 
        {"Mexico City", -6}, {"Detroit", -5}, {"Bahamas", -4}, {"Buenos Aires", -3}, {"Grytviken", -2}, 
        {"Lagoa", -1}, {"Manchester", 0}, {"Amsterdam", 1}, {"Cairo", 2}, {"Baghdad", 3}, 
        {"Dubai", 4}, {"Kabul", 5}, {"Dhaka", 6}, {"Yangon", 6.5}, {"Bangkok", 7}, 
        {"Hong Kong", 8}, {"Seoul", 9}, {"Melbourne", 10}, {"Micronesia", 11}, {"Auckland", 12}
    };

    int totalZones = 25;

    led.Off();

    Buzzer buzzer;
    buzzer.stopBuzzing();

    state = init;
    while(1){    
        switch (state){
            case (init):
                displayInit();
                break;
            case (setTime):
                updateTimeFromPotentiometer();
                displaySetTime();
                break;
            case (currentTime):
                displayCurrentTime();
                break;
            case (worldTime):
                displayWorldTime(leftHand, GlobalClock, timeZones, totalZones);
                break;
            case (stopwatch):
                displayStopwatch();
                break;
            case (countdown):
                switch(countdownState){
                    case(set):
                        buzzer.stopBuzzing();
                        updateCountdownFromPotentiometer();
                        displayCountdown();
                        break;
                    case(counting):
                        buzzer.stopBuzzing();
                        displayCountdown();
                        break;
                    case(complete):
                        state = countdownComplete;
                        break;
                }
                break;
            case (countdownComplete):
                buzzer.startBuzzing(0.0005);
                displayCompleteScreen();
                break;
            default:
                state = init;
        }
    }
}
