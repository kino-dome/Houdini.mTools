#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM

struct Particle
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
        return "Particle";
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

    Particle[] openAll(StructManager aManager)
    {
        Particle particles[];
        int count = aManager -> getCount(mTag);
        for (int i=0; i < count; i++){
            Particle particle;
            particle -> openId(aManager, i);
            push(particles, particle);
        }
        return particles;
    }

    void close(StructManager aManager)
    {   
        aManager -> setAttrib(mTag, "mRad", mPointNumber, mRad);
        aManager -> setAttrib(mTag, "mCol", mPointNumber, mCol);
        aManager -> setAttrib(mTag, "mPos", mPointNumber, mPos);
        aManager -> setAttrib(mTag, "mVel", mPointNumber, mVel);
    }

    /////////////

    void init()
    {
        mRad = 10.0;
        mCol = set(1.0, 0.0, 0.0, 1.0);
        mPos = set(nrandom()*10, nrandom()*10, 0);
        mVel = set(nrandom(), nrandom(), 0);
    }

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

struct ParticleSystem
{

    string      mTag;
    int         mId;
    int         mPointNumber; // won't be an attrib, just some useful field for betwwen opens and closes
    //////////////////////
    //Particle    mParticle[];
    int         mParticlePoints[];
    
    string getTag()
    {
        if (mTag != "")
        return mTag;
        else
        return "ParticleSystem";
    }

    void setup(StructManager aManager)
    {
        mTag = this -> getTag();

        //aManager -> addAttrib(mTag, "mParticles", mParticles);
        aManager -> addAttrib(mTag, "mParticlePoints", mParticlePoints);
        
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

        //mParticles = aManager -> getAttrib(mTag, "mParticles", mPointNumber);
        mParticlePoints = aManager -> getAttrib(mTag, "mParticlePoints", mPointNumber);

    }

    void openId(StructManager aManager; int aId)
    {
        mTag = this -> getTag();
        mId = aId;
        this -> openPointNumber(aManager, aManager -> getPointNumber(mTag, mId));
    }

    void close(StructManager aManager)
    {   
        //aManager -> setAttrib(mTag, "mParticles", mPointNumber, mParticles);
        aManager -> setAttrib(mTag, "mParticlePoints", mPointNumber, mParticlePoints);
    }

    /////////////
}

#endif