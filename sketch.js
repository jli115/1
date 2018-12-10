// server variables
var dataServer;

// Kate's PubNub key
var subKey = 'sub-c-f36ef67a-e6a6-11e8-b820-164499a0198c';

//input variables
var mappedX;

var data;
//name used to sort your messages. used like a radio station. can be called anything
var channelName = "sensorValues";

var img1;
var img2;

function preload(){
  
   img1 = loadImage("2.png");
   img2 = loadImage("1.png");
  
   
}

function setup() 
{
  createCanvas(windowWidth, windowHeight);
  
 

   // initialize pubnub
  dataServer = new PubNub(
  {
    subscribe_key : subKey,  
    ssl: true  //enables a secure connection. This option has to be used if using the OCAD webspace
  });
  
  //attach callbacks to the pubnub object to handle messages and connections
  dataServer.addListener({ message: readIncoming, presence: whoisconnected })
  dataServer.subscribe({channels: [channelName]});
}

function draw() 
{
   textSize(20);
 fill(0);
    text(-7,200,115);
  
}

function touchStarted() {
   x = mouseX,
   y = mouseY;
  if (x>100 && x<450 && y>500 && y<800){ 
      window.open("http://webspace.ocad.ca/~3173625/call/");
  }
    
  if (x>550 && x<900 && y>500 && y<800){ 
      window.open("http://webspace.ocad.ca/~3173625/data/");
  }
            
  if (x>900 && x<1250 && y>500 && y<800){ 
      window.open("http://webspace.ocad.ca/~3173625/chat/");
         
  }
}

function readIncoming(inMessage) //when new data comes in it triggers this function, 
{                               // this works becsuse we subscribed to the channel in setup()
  
  // simple error check to match the incoming to the channelName
  if(inMessage.channel == channelName)
  {
    
      
    background(255);
    noStroke();
    fill(255,0,0);  //read the color values from the message
 

    mappedX=inMessage.message.sensorVal2;
   if(inMessage.message.sensorVal2 <= 300){
    image(img1,0,0,windowWidth, windowHeight);
       text(" ",100,100);
    
}else{
    image(img2,0,0,windowWidth, windowHeight);
    text(" ",100,100)};
    
  }
}

function whoisconnected(connectionInfo)
{

}
function windowResized() 
{
  resizeCanvas(windowWidth, windowHeight); 
}