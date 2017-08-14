#ifndef BALL
#define BALL

// Attributes in the form of ' "mTag_"mPoint" ' - > Point_0_mPos
struct Ball
{
    string      mTag;
    int         mId;
    int         mPointNumber; // won't be an attrib, just some useful field for betwwen opens and closes
    //////////////////////
    float       mRad;
    vector4     mCol;
    vector      mPos;
    vector      mVel;
    
    string getTag()
    {
        if (mTag != "")
        return mTag;
        else
        return "Ball";
    }

    void setup(StructManager aManager)
    {
        mTag = this -> getTag();

        mRad = -1;
        aManager -> addAttrib(mTag, "mRad", mRad);
        mCol = set(0.0, 0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mCol", mCol);
        mPos = set(0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mPos", mPos);
        mVel = set(0.0, 0.0, 0.0);
        aManager -> addAttrib(mTag, "mVel", mVel);
        
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

        mRad = aManager -> getAttrib(mTag, "mRad", mPointNumber);
        mCol = aManager -> getAttrib(mTag, "mCol", mPointNumber);
        mPos = aManager -> getAttrib(mTag, "mPos", mPointNumber);
        mVel = aManager -> getAttrib(mTag, "mVel", mPointNumber);
    }

    void openId(StructManager aManager; int aId)
    {
        mTag = this -> getTag();
        mId = aId;
        this -> openPointNumber(aManager, aManager -> getPointNumber(mTag, mId));
    }

    Ball[] openAll(StructManager aManager)
    {
        Ball balls[];
        int count = aManager -> getCount(mTag);
        for (int i=0; i < count; i++){
            Ball ball;
            ball -> openId(aManager, i);
            push(balls, ball);
        }
        return balls;
    }

    void close(StructManager aManager)
    {   
        aManager -> setAttrib(mTag, "mRad", mPointNumber, mRad);
        aManager -> setAttrib(mTag, "mCol", mPointNumber, mCol);
        aManager -> setAttrib(mTag, "mPos", mPointNumber, mPos);
        aManager -> setAttrib(mTag, "mVel", mPointNumber, mVel);
    }

    /////////////

    void init(float aX, aY, aSpeedLimit)
    {
        mRad = 10.0;
        mCol = set(1.0, 0.0, 0.0, 1.0);
        mPos = set(aX, aY, 0);
        mVel = set(fit01(nrandom(), -aSpeedLimit, aSpeedLimit), fit01(nrandom(), -aSpeedLimit, aSpeedLimit), 0);
    }

    void display(P5 aP5)
    {
        aP5 -> fill(mCol);
        aP5 -> ellipse(mPos.x, mPos.y, mRad, mRad);
    }

    void update(P5 aP5; float aGravity)
    {
        vector grav = set(0, aGravity, 0);
        mVel += grav;
        mPos += mVel;

        if (mPos.y>=aP5.height && mVel.y>0) {
            mVel.y*=-1; 
            mVel.y*=0.8;
        }
        if (mPos.y<=0 && mVel.y<0) {
            mVel.y*=-1;
            mVel.y*=0.8;
        }
        if (mPos.x>=aP5.width && mVel.x>0) {
            mVel.x*=-1;
            mVel.x*=0.8;
        }
        if (mPos.x<=0 && mVel.x<0) {
            mVel.x*=-1;
            mVel.x*=0.8;
        }
    }

    void update(P5 aP5; vector aAccel)
    {
        mVel += aAccel;
        mPos += mVel;


        if (mPos.y>=aP5.height && mVel.y>0) {
            mVel.y*=-1; 
            mVel.y*=0.8;
        }
        if (mPos.y<=0 && mVel.y<0) {
            mVel.y*=-1;
            mVel.y*=0.8;
        }
        if (mPos.x>=aP5.width && mVel.x>0) {
            mVel.x*=-1;
            mVel.x*=0.8;
        }
        if (mPos.x<=0 && mVel.x<0) {
            mVel.x*=-1;
            mVel.x*=0.8;
        }
    }

    void setColor(vector4 aCol)
    {
        mCol = aCol;
    }
    vector getPos()
    {
        return mPos;
    }

}

void connectBalls(P5 aP5; Ball aBall1, aBall2)
{
    vector pos1 = aBall1 -> getPos();
    vector pos2 = aBall2 -> getPos();
    aP5 -> line(set(pos1.x, pos1.y, 0.0), set(pos2.x, pos2.y, 0.0));    
}

#endif