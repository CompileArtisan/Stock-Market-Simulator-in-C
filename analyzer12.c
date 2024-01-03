#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define NUM_COMPANIES 3

struct stats{
    int min;
    int max;
    int avg;
    int sum;
} s[3];


// Function to plot a graph based on the given stock prices
void plotGraph(int speed[]) {
    for (int i = 10; i > 0; i--) {
        printf("%-4d| ",i*10);
        for (int j = 0; j < 10; j++) {
            if (speed[j] >= i) {
                printf(" ||");
            } 
            else {
                printf("   ");
            }
        }
        printf("\n");
    }
    printf("    |-"); 
    for (int i = 0; i < 10; ++i) {
        printf("---");
    }
    printf("\n");  
}

// Array of UI headings
char UItitles1[10][100] = {"STOCK MARKET ANALYZER\n_____________________\n\nSelect scope of analysis\nW-up ; S-down ; Q-quit\n","Company","Sector"};
char UItitles2[10][100] = {"ANALYSIS SCOPE CONFINED TO A COMPANY\n\nSelect Company to Analyze:\nW-up ; S-down ; Q-quit\n","Googil","Microhard","Aaple"};

void sounds(char key){
    if(key!='\r' && key!='b' && key!='B' && key!='q' && key!='Q'){
        Beep(523,50);
    }
    else if(key=='\r'){
        Beep(1047,50);
        Beep(1047,50);
    }
}

// Function to print the user interface and get the selected option
int printUI(char UItitles2[10][100],int totalOptions) {
    int selectedOption = 0;    
    while (1) {
        printf("\033[2J\033[1;1H"); // Clear the console screen
        printf("%s\n",UItitles2[0]); // Print the main heading
        for (int i = 0; i < totalOptions; i++) {
            if (i == selectedOption) {
                printf("-> %s (Selected)\n", UItitles2[i+1]); // Print the selected option with an arrow
            } else {
                printf("   %s\n", UItitles2[i+1]); // Print the other options
            }
        }
        char key = getche(); // Get the user input
        sounds(key);
        if (key == 'w' || key == 'W') {
            selectedOption = (selectedOption - 1 + totalOptions) % totalOptions; // Move the selection up
        } 
        else if (key == 's' || key == 'S') {
            selectedOption = (selectedOption + 1) % totalOptions; // Move the selection down
        } 
        else if (key == 'q' || key == 'Q') {
            printf("uit\n\nThank you for using the program!\n");
            int result = MessageBox(NULL, "Are you sure you want to leave?", "Stock Market Analyzer", MB_YESNO);
            if (result == IDYES) {
                // User clicked 'Yes', exit the program
                printf("\nExiting program...\n");
                exit(1);
            }
        } 
        else if (key == '\r') {
            break; // Break the loop when the user presses enter
        }
    }
    return selectedOption; // Return the selected option
}


// Simulator thread function
void *simulator(void *arg) {
    double prices[NUM_COMPANIES][10];
    for (int i = 0; i < NUM_COMPANIES; i++) {
        for (int j = 0; j < 10; j++) {
            prices[i][j] = (double)(rand() % 1000) / 10.0; // Generate random stock prices
        }
    }
    for (int i = 0; i < NUM_COMPANIES; ++i) {
        s[i].min = INT_MAX; // Initialize min to a high value
        s[i].max = INT_MIN; // Initialize max to a low value
        s[i].avg = 0;
        s[i].sum = 0;
    }
    while (1) {
        FILE *file = fopen("stock_datav12.txt", "w"); // Open the file in write mode
        time_t t = time(NULL);
        fprintf(file, "Stock data as of %s\n", ctime(&t)); // Write the current time to the file
        if (file == NULL) {
            printf("File doesn't exist yet. It has been created.\nPlease run the program again\n\n");
            exit(1); // Exit the program if the file cannot be opened
        }
        srand(time(0));
        for (int i = 0; i < NUM_COMPANIES; i++) {
            fprintf(file, "Company %d:\n", i + 1); // Write the company number to the file
            for (int j = 1 ; j<10 ; j++) {
                prices[i][j-1] = prices[i][j]; // Shift the stock prices by one position
            }
            for (int j = 0; j < 10; ++j) {
                fprintf(file, "%.2f\n", prices[i][j]); // Write the stock prices to the file
            }
            prices[i][9] = 10+ (double)(rand() % 9000) / 100.0; // Generate a new stock price
            fprintf(file, "\n");
        }
        fclose(file); // Close the file
        sleep(2); // Sleep for 2 seconds
    }
    return NULL;
}

int ui=1,comp=0,scope=0;

// Analyzer thread function
void *analyzer(void *arg) {
    while (1) {
        FILE *file = fopen("stock_datav12.txt", "r"); // Open the file in read mode
        if (file == NULL) {
            printf("File has just been created.\nPlease run the program again\n\n");
            exit(1); // Exit the program if the file cannot be opened
        }
        printf("\033[2J\033[1;1H"); // Clear the console screen
        int shares[3][10];
        int company = 0;
        int stock = 0;
        char line[100];
        fgets(line, sizeof(line), file); // Read the first line from the file
        while (fgets(line, sizeof(line), file)) {
            char *end;
            long x = strtol(line, &end, 10); // Convert the line to a long integer
            if (end != line) {
                shares[company][stock] = x; // Store the stock price in the array
                stock++;
                if (stock == 10) {
                    stock = 0;
                    company++;
                }
                if (company == 3) {
                    break;
                }
            }
        }
        fclose(file); // Close the file
        if(ui==1){
            scope=printUI(UItitles1,2); // Print the UI and get the selected option
            if(scope==0){
                comp=printUI(UItitles2,3); // Print the UI and get the selected option   
            }
        }

        for(int i=0 ; i<3 ; i++){
            s[i].min = INT_MAX; // Re-initialize min to a high value
            s[i].max = INT_MIN; // Re-initialize max to a low value
            s[i].sum = 0; // Reset the sum to 0 for each company
            for(int j=0 ; j<10 ; j++){
                s[i].sum+=shares[i][j]; // Calculate the sum of stock prices
                if(shares[i][j] < s[i].min){
                    s[i].min = shares[i][j]; // Update the minimum stock price
                } 
                if(shares[i][j] > s[i].max){
                    s[i].max = shares[i][j]; // Update the maximum stock price
                }
            }
            s[i].avg = s[i].sum / 10; // Calculate the average stock price
        }
        s[comp].avg = s[comp].sum / 10; // Calculate the average stock price
        ui=0;
        if(scope==0){
            printf("\033[2J\033[1;1H"); // Clear the console screen
            
            printf("Analysis of %s\n\n",UItitles2[comp+1]); // Print the selected company name
            int graph[10];
            for(int i = 0; i < 10; i++){
                graph[i] = (int)((shares[comp][i] / 10)+0.5); // Calculate the graph values
            }
            plotGraph(graph); // Plot the graph
            printf("\n\n");
            
            printf("Minimum Stock price: %d\n",s[comp].min);
            printf("Maximum Stock price: %d\n",s[comp].max);
            printf("Average Stock price: %d\n", s[comp].avg);
            printf("Current Stock price: %d\n", shares[comp][9]);
            printf("\nHold/Long Press 'b' to go back to the menu\nPress 'q' to quit\n");
            printf("\033[9;48H");
            printf("\033[10;48H Legend:");
            printf("\033[11;48H X-Coordinate 1 unit = 2.5 seconds");
            printf("\033[13;48H Y-Coordinate 1 unit = $10"); 
            printf("\033[9;47H ___________________________________");
            printf("\033[10;47H|");            printf("\033[10;82H|");
            printf("\033[11;47H|");            printf("\033[11;82H|");   
            printf("\033[12;47H|");            printf("\033[12;82H|");  
            printf("\033[13;47H|");            printf("\033[13;82H|");      
            printf("\033[14;47H|__________________________________|");
            sleep(2); // Sleep for 2 seconds
        }
        else if(scope==1){
            printf("\033[2J\033[1;1H"); // Clear the console screen

            // to print stats of all companies
            printf("Analysis scope confined to the whole sector\n\n\n");
            printf("%-15s %-15s %-15s %-15s %-15s\n________________________________________________________________________________\n",
             "Company", "Min Price", "Max Price", "Avg Price", "Current Price");
            for(int i=0 ; i<3 ; i++){
                printf("%-15s %-15d %-15d %-15d %-15d\n\n", UItitles2[i+1], s[i].min, s[i].max, s[i].avg, shares[i][9]);
            }            
            printf("\n\n");
            
            
            printf("\n\nHold/Long Press 'b' to go back to the main menu\nPress 'q' to quit\n");
            sleep(2); // sleep for 2 seconds
        }
        
    }
    return NULL;
}

// Input checking thread function
void *check_user_input(void *arg) {
    while (1) {
        if(ui==1) continue;
        if (getch() == 'q' || getch() == 'Q') {
            printf("\nExiting program...\n");
            int result = MessageBox(NULL, "Are you sure you want to leave?", "Stock Market Analyzer", MB_YESNO);
            if (result == IDYES) {
                // User clicked 'Yes', exit the program
                printf("\nExiting program...\n");
                exit(1);
            }
        }
        else if (getch() == 'b' || getch() == 'B') {
            ui = 1; // Go back to the menu if the user presses 'b'
        }
    }
    return NULL;
}

int main() {
    printf("\033[2J\033[1;1H"); // Clear the console screen
    MessageBox(NULL, "This program is intended for educational purposes only.\nPrices shown do not reflect actual market prices\n\nPress \'OK\' to continue ", "Disclaimer!", MB_OK);
    pthread_t sim_thread, ana_thread,input_thread;
    pthread_create(&sim_thread, NULL, simulator, NULL); // Create the simulator thread
    pthread_create(&ana_thread, NULL, analyzer, NULL); // Create the analyzer thread
    pthread_create(&input_thread, NULL, check_user_input, NULL); // Create the input checking thread
    pthread_join(sim_thread, NULL); // Wait for the simulator thread to finish
    pthread_join(ana_thread, NULL); // Wait for the analyzer thread to finish
    pthread_join(input_thread, NULL); // Wait for the input checking thread to finish
    return 0;
}