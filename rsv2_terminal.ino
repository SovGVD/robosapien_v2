int IRledPin =  13;    // IR LED connected to digital pin 13
String Serial_data = "";

// Robosapien v2 IR codes http://www.aibohack.com/robosap/ir_codes_v2.htm
// some parts of the code from https://learn.adafruit.com/ir-sensor/using-an-ir-sensor
// waste memory, but useful



long IR_delay_HIGH=3380;
long IR_delay_LOW=780;  // 740...780
long IR_pulse=980;
unsigned long command_counter=0;

//#define DEBUG

int commands_size;

String RScommands_keys[] = {
  "M.stop",
  "O.reset",
  
  "S.sonic",
  "S.vision",
  
  "M.forward",      // MOVE
  "M.backward",
  "M.left",
  "M.right",
  "M.upleft",
  "M.upright",
  "M.downleft",
  "M.downright",

  "LA.up",            // LEFT ARM
  "LA.down",
  "LA.left",
  "LA.right",
  "LA.upleft",
  "LA.upright",
  "LA.downleft",
  "LA.downright",
  "LA.throw",
  "LA.low.pickup",
  "LA.high.pickup",
  "LA.grab",
  "LA.give",
  "LA.roll",
  "LA.drop",
  
  "RA.up",            // RIGHT ARM
  "RA.down",
  "RA.left",
  "RA.right",
  "RA.upleft",
  "RA.upright",
  "RA.downleft",
  "RA.downright",
  "RA.throw",
  "RA.low.pickup",
  "RA.high.pickup",
  "RA.grab",
  "RA.give",
  "RA.roll",
  "RA.drop",

  "BA.up",           // BOTH ARMS
  "BA.down",
  "BA.left",
  "BA.right",
  "BA.upleft",
  "BA.upright",
  "BA.downleft",
  "BA.downright",

  "H.up",           // HEAD
  "H.down",
  "H.left",
  "H.right",
  "H.upleft",
  "H.upright",
  "H.downleft",
  "H.downright",

  
  "O.oops",       // OTHER
  "O.roar",
  "O.hug",
  "O.high5",
  
  "O.left.chop",
  "O.left.push",
  "O.left.kick",
  "O.right.chop",
  "O.right.push",
  "O.right.kick"
};

uint32_t RScommands_code[] = {
  0x3AA,
  0x353,

  0x383,  // sonic sensors ON/OFF
  0x380,  // vision system ON/OFF
  
  0x300,  // move forward
  0x301,  //      backward
  0x302,  //      left
  0x303,  //      right
  0x304,  //      upleft
  0x305,  //      upright
  0x306,  //      downleft
  0x307,  //      downright
  
  0x330,  // left arm forward
  0x331,  //          backward
  0x332,  //          left
  0x333,  //          right
  0x334,  //          upleft
  0x335,  //          upright
  0x336,  //          downleft
  0x337,  //          downright
  0x35C,  //          throw
  0x35D,  //          low pickup
  0x35E,  //          high pickup
  0x35F,  //          grab
  0x360,  //          give
  0x361,  //          roll
  0x36F,  //          drop

  0x328,  // right arm forward
  0x329,  //           backward
  0x32A,  //           left
  0x32B,  //           right
  0x32C,  //           upleft
  0x32D,  //           upright
  0x32E,  //           downleft
  0x32F,  //           downright
  0x355,  //           throw
  0x356,  //           low pickup
  0x357,  //           high pickup
  0x358,  //           grab
  0x359,  //           give
  0x35A,  //           roll
  0x36E,  //           drop

  0x338,  // both arms forward
  0x339,  //           backward
  0x33A,  //           left
  0x33B,  //           right
  0x33C,  //           upleft
  0x33D,  //           upright
  0x33E,  //           downleft
  0x33F,  //           downright

  0x348,  // head up
  0x349,  //      down
  0x34A,  //      left
  0x34B,  //      right
  0x34C,  //      upleft
  0x34D,  //      upright
  0x34E,  //      downleft
  0x34F,  //      downright

  0x35B,
  0x374,
  0x379,   // hug
  0x369,   // high 5

  0x367,   // left chop
  0x368,   //      push
  0x363,   //      kick
  0x366,   // right chop
  0x365,   //       push
  0x362    //       kick

};

int Command_found=-1;
boolean IRcommand[12] = {};

boolean Command_progress=false;
boolean Command_done=false;
 
void setup() {                
  pinMode(IRledPin, OUTPUT);      
  Serial.begin(9600);
  Serial_data.reserve(200);
  Serial.println();
  Serial.println();
  Serial.println("Robosapien V2 terminal.");
  //Serial.println("Robco Industries (TM) 2077.");  // Fallout =)
  Serial.print("Commands total: ");
  commands_size=sizeof(RScommands_code)/sizeof(uint32_t);
  Serial.println(commands_size);
  Serial.println("Type `help` for all commands list. Also easter egg ;)");
}
 
void loop() {
  wait_serial_data();
}

void wait_serial_data() {
  while (Serial.available()) {
    char tmp = (char)Serial.read();
    if (tmp == '\n') {
      do_action(Serial_data);
      Serial_data = "";
    } else {
      Serial_data += tmp;
    }
  }
}

void do_help() {
  for (int i =0; i< commands_size; i++) {
    //Serial.print(i); Serial.print("  ");
    Serial.println(RScommands_keys[i]);
  }
  //Serial.println();
  Serial.println();
}

void do_action (String c) {
  Command_found=-1; // the 0 is first element in array
  if (c=="help") {
    c="";
    do_help();
  } else if (c=="fallout" || c=="robco") {
    c="";
    show_robot();
  } else if (c.substring(0,3)=="raw") {
    
  } else  {
    for (int i =0; i< commands_size; i++) {
      if (RScommands_keys[i] == c) {
        Command_found=i;
        break;
      }
    }
    if (Command_found==-1) {
      Serial.print("UNKNOWN ");
      Serial.println(Serial_data);  
    } else {
      #if defined (DEBUG)
       Serial.println(Command_found);
       Serial.println(RScommands_keys[Command_found]);
       Serial.println(RScommands_code[Command_found],BIN);
      #endif
      for (int i=0;i<10;i++) {
        IRcommand[(11-i)]=bitRead(RScommands_code[Command_found],i);
      }
      if (command_counter>4294967290) command_counter=0;  // prevent overflow
      command_counter++;
      Serial.print("COMMAND-");
      Serial.print(RScommands_keys[Command_found]);
      Serial.print("-");
      Serial.print(RScommands_code[Command_found],BIN);
      Serial.print("-ID-");
      Serial.println(command_counter);
      do_IR(IRcommand,command_counter);
    }
  }
}

void do_IR (boolean cmd[12],long cmd_id) {
  pulseIR(7020,760);
  for (int i=1;i<12;i++) {
    #if defined (DEBUG)
      Serial.println(cmd[i]);
    #endif
    if (cmd[i]==0) {
      #if defined (DEBUG)
        Serial.println("LOW");
      #endif
      pulseIR(IR_pulse, IR_delay_LOW);
    } else {
      #if defined (DEBUG)
        Serial.println("HIGH");
      #endif
      pulseIR(IR_pulse, IR_delay_HIGH);
    }
  }
  pulseIR(IR_pulse, 6440);
  Serial.print("DONE-");
  Serial.println(command_counter);
}


 
// This procedure sends a 38KHz pulse to the IRledPin 
// for a certain # of microseconds. We'll use this whenever we need to send codes
void pulseIR(long microsecs, long Delay_uS) {
  // we'll count down from the number of microseconds we are told to wait
 
  cli();  // this turns off any background interrupts
 
  while (microsecs > 0) {
    // 38 kHz is about 13 microseconds high and 13 microseconds low
   digitalWrite(IRledPin, HIGH);  // this takes about 3 microseconds to happen
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
   digitalWrite(IRledPin, LOW);   // this also takes about 3 microseconds
   delayMicroseconds(10);         // hang out for 10 microseconds, you can also change this to 9 if its not working
 
   // so 26 microseconds altogether
   microsecs -= 26;
  }
  sei();  // this turns them back on
  delayMicroseconds(Delay_uS);
}



 void show_robot() {
 Serial.println("               _-=-_ ");
Serial.println("              /     \\ ");
Serial.println("             /   (=) \\ ");
Serial.println("            /_  o |  _\\ ");
Serial.println("           /  \\=|=|=/  \\ ");
Serial.println("          /   |=|=*=|   \\ ");
Serial.println("         /\\   |=+=|=|   /\\ ");
Serial.println("         |\\    \\|_|/    /| ");
Serial.println("        /\\               /\\ ");
Serial.println("     __/  \\             /  \\__ ");
Serial.println("   _   _   \\    ===    /   _   _ ");
Serial.println("  /     \\   \\__/   \\__/   /     \\ ");
Serial.println(" /__    |                 |    __\\ ");
Serial.println(" |  \\__/   - - - - - - -   \\__/  | ");
Serial.println("  \\____/\\       ====       /\\____/ ");
Serial.println("  <____> \\     /____\\     / <____> ");
Serial.println("  <____>  \\   /______\\   /  <____> ");
Serial.println(" <____>    \\_/<______>\\_/    <____> ");
Serial.println(" <____>       <______>       <____> ");
Serial.println(" /    \\    _-_<______>_-_    /    \\ ");
Serial.println("/     |   /   /      \\   \\   |     \\ ");
Serial.println("|o    |   ====\\      /====   |    o| ");
Serial.println("|     |   \\   /\\____/\\   /   |     | ");
Serial.println("\\==== /   |___|      |___|   \\ ====/ ");
Serial.println(" \\___/    <___>      <___>    \\___/ ");
Serial.println(" ( ( )    <___>      <___>     ( |) ");
Serial.println("          <___>      <___> ");

 
 }

 
