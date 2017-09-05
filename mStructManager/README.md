# mStructManager

**This is a workaround to enable Object Oriented Programming in VEX to some extent. The repo contains a VEX code for a struct named "StructManager". Using this struct and some minimal handiwork, you can have your own defined structs (close to classes) live on a geometry stream in Houdini**

**How it works?**

The main idea behind the system is that by having a couple of attributes on detail and point level you can keep track of any number of structs arrays and open/close them whenever needed. You just need to define some code (below) in the beginning of your struct definition to enable "StructManager" to understand the struct and be able to track it.

The StructManager struct uses houdini points as array elements. each element knows of what type it is (using "mAllTags" attribute) and which instance of all the array it is (using "mIds" attribute). From here on StructManager can easily call the instance it wants from the array and write to/read it. It's important to know that these created points have no SOP related data (vertex, polys etc.) so Houdini will not use their data and you can have them safely beside the geometry points on a stream.


**Prep**
Definitely check the sample to see this in action (use a geometry spreadsheet to see the changes). I also used this in one of the samples for the [P5InH](https://github.com/kino-dome/P5InH) asset to simulate two "Ball" classes move around in the screen and draw their trails using P5InH.

There are some steps you need to take first:

1- Make "StructManager.h" file (and any other vex struct defintions you need) available in project's "vex" folder (or Houdini's own directory for vex includes).

2- In Houdini you need to first initialize the StructManager. Wrangeling this will do it for you:
```C
#include <StructManager.h>

StructManager manager;
manager -> setup(geoself());
```
This sets up the StructManager (could be used on any inputs) and adds a couple of detail and point attributes to the geometry.

3- Then for any struct you have, you need to set it up for the StructManager. For a "Ball" class you'd wrangle:
```C
#include <StructManager.h>
#include <Ball.h>

StructManager manager = openStructManager(geoself());

Ball ball;
ball -> setup(manager);
```

4- From here on, you can add/remove struct instances to the geometry as well as reading/writing the instances with open and close functions. Here I add 22 Balls, initialize, color them and save their info on the geometry:

```C
#include <StructManager.h>
#include <Ball.h>

StructManager manager = openStructManager(geoself());

for (int i=0; i<22; i++){
    Ball ball;
    ball -> add(manager, -1); // add a new instance of Ball to the manager
    ball -> init(600.0, 100.0, 5.0); //initializes the ball instance
    ball -> setColor( set(0.0, 0.0, 33.0 / 255.0, alph)); // color it
    ball -> close(manager); // saves this ball's data to the geometry. IMPORTANT TO HAVE !
}

manager -> close();
```

You can further manipulate the instances whenever you wanted down the stream. like this:
```C
#include <StructManager.h>
#include <Ball.h>

StructManager manager = openStructManager(geoself());


Ball ball = Ball(); 
int count = manager -> getCount(ball -> getTag());

for (int i=0; i<count; i++){
    ball -> openId(manager, i);
    ball -> setColor(nrandom());
    ball -> close(manager);
}

manager -> close();
```
5- Whenever you were done with the instances you can remove them from the geometry:
```C
#include <StructManager.h>
#include <Ball.h>

StructManager manager = openStructManager(geoself());

Ball ball = Ball(); 
int count = manager -> getCount(ball -> getTag());
for (int i=0; i<count; i++){
    ball -> openId(manager, i);
    ball -> remove(manager, 1); //second parameter defines if the point should be also removed
}

manager -> close();
```

6- Last but not least, you need to have this bit of code for any struct you want to use with this system. You can duplicate most of the code except for the getTag() function where you'll write down your struct's name and setup()/opnePointNumber()/close() functions where you'll need to change according to your struct's data members:

```C
    string      mTag;
    int         mId;
    int         mPointNumber;
    ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
    float       mRad; 
    vector4     mCol;
    vector      mPos;
    vector      mVel;
    ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
    
    string getTag()
    {
        if (mTag != "")
        return mTag;
        else
        ///////////////// YOUR CLASS' NAME
        return "Ball";
        ///////////////// YOUR CLASS' NAME
    }

    void setup(StructManager aManager)
    {
        mTag = this -> getTag();
        
        ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
        mRad = -1;
        aManager -> addAttrib(mTag, "mRad", mRad);
        mCol = set(0.0, 0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mCol", mCol);
        mPos = set(0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mPos", mPos);
        mVel = set(0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mVel", mVel);
        ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
    }

    void add(StructManager aManager; int aPointNumber)
    {
        mTag = this -> getTag();
        mPointNumber = aManager -> addMember(mTag, aPointNumber);
    }

    void remove(StructManager aManager; int aShouldRemovePoint)
    {
        aManager -> removeMember(mTag, mId, aShouldRemovePoint);
    }

    void openPointNumber(StructManager aManager; int aPointNumber)
    {
        mTag = this -> getTag();
        mPointNumber = aPointNumber;
        mId = aManager -> getId(mPointNumber, mTag);
        
        ////////////////////// ACCORDING TO YOUR STRUCT
        mRad = aManager -> getAttrib(mTag, "mRad", mPointNumber);
        mCol = aManager -> getAttrib(mTag, "mCol", mPointNumber);
        mPos = aManager -> getAttrib(mTag, "mPos", mPointNumber);
        mVel = aManager -> getAttrib(mTag, "mVel", mPointNumber);
        ////////////////////// ACCORDING TO YOUR STRUCT
    }

    void openId(StructManager aManager; int aId)
    {
        mTag = this -> getTag();
        mId = aId;
        this -> openPointNumber(aManager, aManager -> getPointNumber(mTag, mId));
    }

    void close(StructManager aManager)
    {   
        ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
        aManager -> setAttrib(mTag, "mRad", mPointNumber, mRad);
        aManager -> setAttrib(mTag, "mCol", mPointNumber, mCol);
        aManager -> setAttrib(mTag, "mPos", mPointNumber, mPos);
        aManager -> setAttrib(mTag, "mVel", mPointNumber, mVel);
        ////////////////////// ACCORDING TO YOUR STRUCT DEFINITION
    }
```

