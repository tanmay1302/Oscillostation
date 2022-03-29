// minimize function calls !!!!!!!!!!!
// display engine
/*
  |+ y axis
  |                                 x-> drawing origin
  |
  |  o o   o    o     o
  |
  |
  |
  |               /
  x _____________o__________________________ +x axis

*/

bool line_pixels[256]; // 8 bit dac
//int points[256];

//void sort()
int mini(int x, int y) {
 int res = x;
  if (x > y) res = y;
  return res;
}

int maxi(int x, int y) {
  int res = x;
  if (x < y )res = y;
   return res;
}

void limit_angle (int &angle) { // limits angle bw 0 and 360
  // 0-360 conv
  if (angle < 0) angle += 360 * (abs(angle) / 360 + 1);
  if (angle > 360) angle %= 360;
}
int num_points = 0, points[255];
class shapes {
  public:
    int chars[10], i;
    int curr_y, x0, x1;
    // int x[256]; // curr_y intercepts
    shapes() {
      // constructor add stuff
    }
    ~shapes() {
      // desctructor ad stuff if feeling lucky : )
    }

    void cpy2array16 (int a[], int b[], int n) {
      for (i = 0; i < n; i++) a[i] = b[i];
    }

    int sgn(int x) {
      return (x > 0) - (x < 0);
    }


    void circle() {
      // add fill
      // 1- > centx 2-> centy 3-> radius , 4-> start angle , 5_> end angle  .. fill ? : 1/0
      //Serial.println(curr_y);
      if (pow(chars[3], 2) - pow(curr_y - chars[2], 2) >= 0 ) { // lets stay real!
        x0 = chars[1] - sqrt(pow(chars[3], 2) - pow(curr_y - chars[2], 2));
        x1 = chars[1] + sqrt(pow(chars[3], 2) - pow(curr_y - chars[2], 2)); // calc two intercepts
        //int angle1 = 180.0 / PI * atan((curr_y * 1.0 - chars[2]) / (x0 - chars[1])) ? x0 - chars[1] != 0 : 90 * sgn(curr_y-chars[2]); // calc angle + catch exploding denominators!!
        //int angle2 = 180.0 / PI * atan((curr_y * 1.0 - chars[2]) / (x1 - chars[1])) ? x1 - chars[1] != 0 : 90 * sgn(curr_y-chars[2]);

        if (x0 >= 0 && x0 <= 255)
          points[num_points++] =  x0;
        if (x1 >= 0 && x1 <= 255 )
          points[num_points++] =  x1;

      }
    }

    void linepal() { // line in start-point angle length form
      // *** deal with 0 / 90 angle cases **
      // x1 y1 angle(degrees) length

      if (curr_y <= maxi(chars[2] + chars[4]*sin(180.0 * chars[3] / PI), chars[2]) && curr_y >= mini(chars[2] + chars[4]*sin(180.0 * chars[3] / PI), chars[2]) ) { //current y between two extreme y values
        // check if the current y coordinate falls inside the drawing range                                                                     // for drawing purposes and this is it !!
        if (chars[3] != 90 || chars[3] != 0) { // reg non 90 and 0 angle cases
          x0 = chars[1] + (chars[4]) * cos((180.0 * chars[3]) / PI);
          if (x0 <= 255 && x0 >= 0)
            points[num_points++] = x0;
        } // check bounds }

        else if (chars[3] == 90 && mini(chars[2] + chars[4], chars[4]) <= curr_y && curr_y <= maxi(chars[2] + chars[4], chars[4]) ) { // check if the x falls in the screen y = chars[1] type line
          if (chars[1] <= 255 && chars[1] >= 0)
            points[num_points++] = chars[1];

        }
        else if (chars[3] == 0 && chars[2] == curr_y) // x = chars [2] type line
        {
          i = mini(chars[1], chars[1] + chars[4]);
          i = i ? i >= 0 : 0; // constrain to 0 +
          while (i <= maxi(chars[1], chars[1] + chars[4]) && i <= 255 ) // horizontal line
            points[num_points++] = i;
        }

      }
     
    }
    void linep2p() { // line in point 2 point form
      // x1 y1 x2 y2
      // 1  2  3  4
      // 30,30,30,10
      // assuming singular intercepts (large angle and dims ) .. ( look  for solns for small angles, quantizations!!!!)
     
      //Serial.print(chars[2]);
      //Serial.println(chars[4]);
      if (chars[1] == chars[3]){
        // Serial.println("v");// for vertical 
        if (mini(chars[2], chars[4]) <= curr_y && curr_y <= maxi(chars[2], chars[4])) // if current y within drawing range
         if (chars[1] <= 255 && chars[1] >= 0) // if x is constrained
            points[num_points++] = chars[1]; // put a point at the specified x
      }
      else if (chars[2] == chars[4]){ // horz .. will be triggered at the app instance
          i = mini(chars[1], chars[3]);
          if (i<0)i=0;
          while (i <= maxi(chars[1], chars[3]) && i <= 255 ) // horizontal line
            points[num_points++] = i++;   
         
      }
      else{
         // y= mx + c         form .. single point
          x0 = chars[1] + (chars[1] * 1.0 - chars[3]) * (curr_y - chars[2]) / (chars[2] - chars[4]); // simple 2 point line form
          if (x0 <= 255 && x0 >= 0)
            points[num_points++] = x0;
            //Serial.println("n");
        }
        

    }
    void ellipse() { // add later.... if feeling lucky...
    }


    void read_primitives (int attributes[10], int y)  { // reads the shape characteristics
      // start translation...
      // primitives
      curr_y = y;
      cpy2array16 (chars, attributes, 10);
      if (chars[0] == 0) circle ();
      else if (chars[0] == 1) linepal();
      else if (chars[0] == 2) {
        //Serial.println("lsel");
        linep2p();
      }
      else if (chars[0] == 3) ellipse();

    }

    int bottom_y(int att[10]) // returns the lower y coordinate of the asked
    {
      int retvar ; // dummy : )
      switch (att[0])
      {
        case 0:
          retvar = att[2] - att[3]; break;
        case 1:
          retvar = mini(att[2], att[4] * sin(180.0 * chars[3] / PI)); break;// if I decide to use signed vars...break;
        case 2:
          retvar = mini(att[2], att[4]); break;
      }
      return retvar;
    }

    int top_y(int att[10]) // returns the lower y coordinate {
    {
      int retvar ; // dummy : )
      switch (att[0])
      {
        case 0:
          retvar = att[2] + att[3]; break;

        case 1:
          retvar = maxi(att[2], att[4] * sin(180.0 * chars[3] / PI)); break;// if I decide to use signed vars... i used ff f f f

        case 2:
          retvar = maxi(att[2], att[4]); break;// signed value implementation


      }
      // add for right x and left x
      return retvar;
    }




} s;

class digitalToanalogConverter {
    // const
    // dest
    int pin_startx; // starting pin
    int pin_starty; // starting pin

  public:
    // declare copy function
    void draw(int x, int y) {
      // A0->LSB A7->MSB
      // B0->LSB B7->MSB

      //DAC.Set(x, y); //delay(0);
    //  GPIOA->ODR = (int16_t)x; // automatically picks up the binary eq in translation// stm32 specific
     // GPIOB->ODR = (int16_t)y;


    }

} DAC0;


void setup() {
  // put your setup code here, to run once:
  //SPI.begin();
  Serial.begin(115200);
  //Serial.print(mini(20,20));
 pinMode(PA0,OUTPUT);pinMode(PA1,OUTPUT);pinMode(PA2,OUTPUT);pinMode(PA3,OUTPUT);pinMode(PA4,OUTPUT);pinMode(PA5,OUTPUT);pinMode(PA6,OUTPUT);pinMode(PA7,OUTPUT);
  pinMode(PB0,OUTPUT);pinMode(PB1,OUTPUT);pinMode(PB2,OUTPUT);pinMode(PB3,OUTPUT);pinMode(PB4,OUTPUT);pinMode(PB5,OUTPUT);pinMode(PB6,OUTPUT);pinMode(PB7,OUTPUT);
 // pinMode(PA8,OUTPUT);pinMode(PA9,OUTPUT);pinMode(PA10,OUTPUT);pinMode(PA11,OUTPUT);pinMode(PA12,OUTPUT);pinMode(PA13,OUTPUT);pinMode(PA14,OUTPUT);pinMode(PA15,OUTPUT);

}


int x, num_objects = 2, frame_objects[][10] = {{2,0,80,100,0},{0, 100, 100, 60, 0, 360}}; // implement double ended queue on frame_objects
int by, ty, i = 0;
int starting = 0, ending, y;
int k = 0;


void sort_objects (int objects[][10], int num_objects) { // ascending
  int i, j, ex[10], k; // delete objects whose top y < 0 (completely out of drawable frame)
  for (i = 0; i < num_objects; i++) // dumb dumb logic
    for (j = 0; j < num_objects - 1 - i; i++) {
      if (s.bottom_y(objects[j]) > s.bottom_y(objects[j + 1])) { // unacceptable !  we shall exchange u for a new one : - )
        for (k = 0; i < 10; k++) ex[k] = objects[j][k];
        for (k = 0; i < 10; k++) objects[j][k] = objects[j + 1][k];
        for (k = 0; i < 10; k++) objects[j + 1][k] = ex[k];
      }
    }
}

void sort (int point[], int num, bool dir = 0) { //  by def asc set dir = 1 for desc
  int i, j;
  for (i = 0; i < num; i++)
    for (j = 0; j < num - i - 1; j++)
      if ((1 - 2 * dir)*point[j] > point[j + 1] * (1 - 2 * dir))
      {
        point[j] += point[j + 1];
        point[j + 1] = point[j] - point[j + 1];
        point[j] = point[j] - point[j + 1];
      }
}

int p = 0; // object counter
bool out = 0;

void loop() {

//Serial.print("main");
  // sort _ frame objects and pop those entries which are out of bounds .. use quicksort
  sort_objects(frame_objects, num_objects); // use s.bottom_y(frame_objects[i])
  
  starting = 0; // reset object counter
  // drawing part
  for (y = 0; y <= 255; y++) { // scan vertically .. bottom to top
    
    num_points = 0; //reset the number of points
    p = starting; // reset ..
    while (p < num_objects) { //scan shapes to fill the current row with their intercepts 
        //Serial.println(s.bottom_y(frame_objects[0]));
        //Serial.println(s.top_y(frame_objects[0]));
        
      if (s.top_y(frame_objects[p]) < y) { // for drawing .. bottom <= y <= top  ... corollary -> bottom > y || top < y
        starting++; // the object cannot be read
          // Serial.println("b");
      }
      else if (y < s.bottom_y(frame_objects[p])) { // bottom of the next object above the current y; // pixels for current row are allocated
         //t  
         //Serial.print("..");
        break;// out = 1; // objects are arranged in ascending order of bottom y's ... hence the next object's bottom y will obv. be above the current object's bottom y
      }
      // increments only if the next object is supposed to be read
    
      s.read_primitives(frame_objects[p], y); // generate line_pixels array for current y
      // reverse sorting order after each iteration to effectively half the scan line retractions
      p++; // skip to the next object having higher bottom y coordinate
      
    }
    sort(points, num_objects, p % 2); // sort the generated points array to prevent jitter and improve pointing times
    if (num_points)// skip empty lines
     GPIOB->ODR = (GPIOB->IDR & 0xFF00) | y; // set y // port manipulation
    
    for (k = 0; k < num_points; k++){ // sweep x
     GPIOA->ODR = (GPIOA->IDR & 0xFF00) | points[k]; // automatically picks up the binary eq in translation// stm32 specific
      delayMicroseconds(1);// wait for capture

    }

  }

}
/*

   void draw_frame()
  {
    sort_objects(frame_objects,num_objects); // use s.bottom_y(frame_objects[i])
  p = 0; // reset object counter
  // drawing part
  for (y = 0; y <= 255; y++){ // scan vertically
   num_points = 0; //reset the number of points
    out = 0;
    p = 0;
    while (p < num_objects && !out) { //scan shapes
      by = s.bottom_y(frame_objects[p]);

      sort(points,num_objects,p%2); // sort the generated points array to prevent jitter and improve pointing times
  // reverse sorting order after each iteration to effectively half the scan line retractions

     if (y < by) // bottom of the next object above the current y; // pixels for current row are allocated
        out = 1; // break from this  loop : )
     // increments only if the next object is supposed to be read
      else{

      s.read_primitives(frame_objects[p], y); // generate line_pixels array for current y
      p++;
      }
    }

        if (num_points)// skip empty lines
        GPIOB->ODR = (GPIOB->IDR&0xFF00)|((int)constrain(y,0,255));// set y // port manipulation
    for (k = 0; k < num_points; k++) // sweep x
        {GPIOA->ODR = (GPIOA->IDR&0xFF00)|((int)constrain(points[k],0,255)); // automatically picks up the binary eq in translation// stm32 specific
        delayMicroseconds(5); // wait for capture

        }

  }
  }


*/
