#ifndef STRUCT_MANAGER
#define STRUCT_MANAGER

struct StructManager
{
    string      mTags[];
    int         mIds[];
    string      mAllTags; // workaround: there is a bug with string arrays on detail with my version of houdini 15.0.274

    int         mGeometry; //after a manager is opened or gotten, it'll know the geometry who owns it

    // string[] bug workaround
    void pushTag(string aTag)
    {
        append(mAllTags, " " + aTag);
    }

    void pushTag(string aAllTags, aTag)
    {
        append(aAllTags, " " + aTag);
    }

    // string[] bug workaround
    string[] getTagList()
    {
        string tagList[] = split(mAllTags, " ");
        return tagList;
    }

    string[] getTagList(string aAllTags)
    {
        string tagList[] = split(aAllTags, " ");
        return tagList;
    }

    string makeAllTags()
    {
        string allTags = "";
        foreach(string tag; mTags){
            this -> pushTag(allTags, tag);
        }
        return allTags;
    }

    string makeAllTags(string aTags[])
    {
        string allTags = "";
        foreach(string tag; aTags){
            this -> pushTag(allTags, tag);
        }
        return allTags;
    }

        // UTIL
    string getAttribName(string aTag; string aVarName)
    {
        string attribName = aTag + "_" +  aVarName;
        return attribName;
    }

    int getCount(string aTag)
    {
        string tags[] = this -> getTagList();
        int tagIndex = find(tags, aTag);
        if (tagIndex < 0){
            return -1;
        }
        else{
            return (mIds[tagIndex] + 1);
        }
    }

    int getPointNumber(string aTag; int aId)
    {
        int allPoints[];
        int allIds[];
        for (int i=0; i<npoints(mGeometry); i++){
            string pointAllTags = attrib(mGeometry, "point", "mAllTags", i);
            string pointTags[] = this -> getTagList(pointAllTags);
            int pointIds[] = attrib(mGeometry, "point", "mIds", i);
            int findIndex = find(pointTags, aTag);
            if (findIndex < 0){
                continue;
            }
            else {
                push(allPoints, i);
                push(allIds, pointIds[findIndex]);
            }
        }

        int findIndex = find(allIds, aId);
        if (findIndex <0 ) return -1;
        else return allPoints[findIndex];
    }

    //sorted by increasing Ids
    int getAllPoints(string aTag)
    {
        int allPoints[];
        int allIds[];
        for (int i=0; i<npoints(mGeometry); i++){
            string pointAllTags = attrib(mGeometry, "point", "mAllTags", i);
            string pointTags[] = this -> getTagList(pointAllTags);
            int pointIds[] = attrib(mGeometry, "point", "mIds", i);
            int findIndex = find(pointTags, aTag);
            if (findIndex < 0){
                continue;
            }
            else {
                push(allPoints, i);
                push(allIds, pointIds[findIndex]);
            }
        }
        allPoints = reorder(allPoints, argsort(allIds));
        return allPoints;
    }

    // TAG AND ID ATTRIBS

    int getId(int aPointNum; string aTag)
    {
        string pointAllTags = attrib(mGeometry, "point", "mAllTags", aPointNum);
        string pointTags[] = this -> getTagList(pointAllTags);
        int pointIds[] = attrib(mGeometry, "point", "mIds", aPointNum);
        int findIndex = find(pointTags, aTag);
        if (findIndex < 0){
            return -1;
        }
        else{
            return pointIds[findIndex];
        }
    }

    string[] getTagsAttrib (int aPointNum)
    {
        string pointAllTags = attrib(mGeometry, "point", "mAllTags", aPointNum);
        string pointTags[] = this -> getTagList(pointAllTags);
        return pointTags;
    }

    void setTags(int aPointNum; string aTags[])
    {
        setpointattrib(mGeometry, "mAllTags", pointNum, this -> makeAllTags(aTags));
        setattrib(mGeometry, "point", "mTags", aPointNum, 0, aTags, "set");
    }

    void setId(int aPointNum; string aTag; int aId)
    {
        string pointAllTags = attrib(mGeometry, "point", "mAllTags", aPointNum);
        string pointTags[] = this -> getTagList(pointAllTags);
        int pointIds[] = attrib(mGeometry, "point", "mIds", aPointNum);
        int findIndex = find(pointTags, aTag);
        if (findIndex < 0){
            return;
        }
        else{
            pointIds[findIndex] = aId;
        }
        setattrib(mGeometry, "point", "mIds", aPointNum, 0, pointIds, "set");
    }

    void close()
    {
        setattrib(mGeometry, "detail", "mTags", 0, 0, mTags, "set");
        setattrib(mGeometry, "detail", "mIds", 0, 0, mIds, "set");
        setattrib(mGeometry, "detail", "mAllTags", 0, 0, mAllTags, "set");
        //Tags to delete
        string tagList[] = this -> getTagList();
        string tagsToDelete = "";
        foreach(string tag; tagList){
            append(tagsToDelete, tag + "*" + " ");
        }
        setattrib(mGeometry, "detail", "mTagsToDelete", 0, 0, tagsToDelete, "set");
    }

    void setup(int aGeometry)
    {
        int s;
        mAllTags = "";
        mGeometry = aGeometry;
        if (attribtypeinfo(mGeometry, "detail", "mTags") == ""){
            //it's our first time, need to make the attribs
            addattrib(mGeometry, "detail", "mTags", {});
            addattrib(mGeometry, "detail", "mIds", {});
            addattrib(mGeometry, "detail", "mAllTags", mAllTags);
            addattrib(mGeometry, "detail", "mTagsToDelete", "");

            addattrib(mGeometry, "point", "mTags", {});
            addattrib(mGeometry, "point", "mAllTags", "");
            addattrib(mGeometry, "point", "mIds", {});
        }
        this -> close();
    }

    void open(int aGeometry)
    {
        mGeometry = aGeometry;
        mIds = attrib(mGeometry, "detail", "mIds", 0);
        mTags = attrib(mGeometry, "detail", "mTags", 0);
        mAllTags = attrib(mGeometry, "detail", "mAllTags", 0);
    }

    int addMember(string aTag; int aPointNum) //uses the given aPointNum and set's that as a member
    {
        int pointNum;
        
        if (aPointNum < 0){
            pointNum = addpoint(mGeometry, set(100000, 100000, 100000));
        }
        else {
            pointNum = aPointNum;
        }

        string tags[] = this -> getTagList();
        int tagIndex = find(tags, aTag);
        if (tagIndex<0){
            //we have to make a new member tag
            this -> pushTag(aTag);
            push(mTags, aTag);
            push(mIds, 0);

            string pointAllTags = attrib(mGeometry, "point", "mAllTags", pointNum);
            string pointTags[] = this -> getTagList(pointAllTags);
            int pointIds[] = attrib(mGeometry, "point", "mIds", pointNum);
            int findIndex = find(pointTags, aTag);
            if (findIndex < 0){
                push(pointTags, aTag);
                push(pointIds, 0);
            }
            else{
                pointIds[findIndex] = mIds[tagIndex];
            }
            setpointattrib(mGeometry, "mAllTags", pointNum, this -> makeAllTags(pointTags));
            setpointattrib(mGeometry, "mTags", pointNum, pointTags);
            setpointattrib(mGeometry, "mIds", pointNum, pointIds);
        }
        else {
            //add another instance
            mIds[tagIndex] += 1;

            string pointAllTags = attrib(mGeometry, "point", "mAllTags", pointNum);
            string pointTags[] = this -> getTagList(pointAllTags);            int pointIds[] = attrib(mGeometry, "point", "mIds", pointNum);
            int findIndex = find(pointTags, aTag);
            if (findIndex < 0){
                push(pointTags, aTag);
                push(pointIds, mIds[tagIndex]);
            }
            else{
                pointIds[findIndex] = mIds[tagIndex];
            }

            setpointattrib(mGeometry, "mAllTags", pointNum, this -> makeAllTags(pointTags));
            setpointattrib(mGeometry, "mTags", pointNum, pointTags);
            setpointattrib(mGeometry, "mIds", pointNum, pointIds); 
        }

        return pointNum;
    }

    int addMember(string aTag) //returns the point number of the newly created point
    {
        int pointNum = this -> addMember(aTag, -1);
        return pointNum;
    }

    void removeMember(string aTag; int aId, aShouldRemovePoint) // destroys the member and optionally it's parent point along with it
    {
        string tags[] = this -> getTagList();
        int tagIndex = find(tags, aTag);
        if (tagIndex < 0) return;
        int pointNum = this -> getPointNumber(aTag, aId);

        if (aShouldRemovePoint){
            removepoint(mGeometry, pointNum);
        }
        else{
            string pointAllTags = attrib(mGeometry, "point", "mAllTags", pointNum);
            string pointTags[] = this -> getTagList(pointAllTags);
            int pointIds[] = attrib(mGeometry, "point", "mIds", pointNum);
            int findIndex = find(pointTags, aTag);
            if (findIndex < 0){
                return;
            }
            else {
                removeindex(pointTags, findIndex);
                removeindex(pointIds, findIndex);
            }
            setpointattrib(mGeometry, "mAllTags", pointNum, this -> makeAllTags(pointTags));
            setpointattrib(mGeometry, "mTags", pointNum, pointTags);
            setpointattrib(mGeometry, "mIds", pointNum, pointIds);
        }

        //remove id and sort mIds
        int count = mIds[tagIndex] + 1;
        for (int i= aId + 1; i< count ; i++){
            int pNum = this -> getPointNumber(aTag, i);
            string pointAllTags = attrib(mGeometry, "point", "mAllTags", pNum);
            string pointTags[] = this -> getTagList(pointAllTags);
            int pointIds[] = attrib(mGeometry, "point", "mIds", pNum);
            int findIndex = find(pointTags, aTag);
            pointIds[findIndex] --;
            setpointattrib(mGeometry, "mIds", pointNum, pointIds);
        }
        mIds[tagIndex] --;
        
        if (mIds[tagIndex] < 0){
            removeindex(mIds, tagIndex);
            removeindex(tags, tagIndex);
            mAllTags = this -> makeAllTags(tags);
        }
        
    }



    // ADD attrib

    string addAttrib(string aTag; string aVarName; float aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; int aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector2 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector4 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix2 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix3 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; string aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, aVar);
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; int aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; string aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; float aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector2 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; vector4 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix2 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix3 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    string addAttrib(string aTag; string aVarName; matrix aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        if (haspointattrib(mGeometry, attribName)) return attribName;
        addattrib(mGeometry, "point", attribName, {});
        return attribName;
    }

    // GET ATTRIB

    float getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        float result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    int getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        int result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    string getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        string result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector2 getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector2 result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector4 getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector4 result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix2 getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix2 result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix3 getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix3 result = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    int[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        int result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    float[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        float result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    string[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        string result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector2[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector2 result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    vector4[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        vector4 result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix2[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix2 result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }

    matrix3[] getAttrib(string aTag; string aVarName; int aPointNum)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        matrix3 result[] = attrib(mGeometry, "point", attribName, aPointNum);
        return result;
    }
    

    // SET ATTRIB
    string setAttrib(string aTag; string aVarName; int aPointNum; float aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; int aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector2 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector4 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix2 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix3 aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; string aVar)
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; int aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; string aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; float aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector2 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; vector4 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix2 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix3 aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }

    string setAttrib(string aTag; string aVarName; int aPointNum; matrix aVar[])
    {
        string attribName = this -> getAttribName(aTag, aVarName);
        setattrib(mGeometry, "point", attribName, aPointNum, 0, aVar, "set");
        return attribName;
    }
}


StructManager openStructManager(int aGeometry)
{
    StructManager manager = StructManager();
    manager -> open(aGeometry);
    return manager;
}

#endif

// TEST

#ifndef TEST_STRUCT_MANAGER
#define TEST_STRUCT_MANAGER

struct Array2Dint
{
    string      mTag;
    int         mId;
    int         mPointNumber;
    ///////////////////////
    int         mValues[];
    int         mDividerIndices[];

    string getTag()
    {
        if (mTag != "")
        return mTag;
        else
        return "Array2Dint";
    }

    void setup(StructManager aManager)
    {
        mTag = this -> getTag();

        mValues = {};
        aManager -> addAttrib(mTag, "mValues", mValues);
        mDividerIndices = {};
        aManager -> addAttrib(mTag, "mDividerIndices", mDividerIndices);
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

        mValues = aManager -> getAttrib(mTag, "mValues", mPointNumber);
        mDividerIndices = aManager -> getAttrib(mTag, "mDividerIndices", mPointNumber);
    }

    void openId(StructManager aManager; int aId)
    {
        mTag = this -> getTag();
        mId = aId;
        this -> openPointNumber(aManager, aManager -> getPointNumber(mTag, mId));
    }

    void close(StructManager aManager)
    {        
        aManager -> setAttrib(mTag, "mValues", mPointNumber, mValues);
        aManager -> setAttrib(mTag, "mDividerIndices", mPointNumber, mDividerIndices);
    }

    /////////////////////////////////

    void append(int aArray[])
    {
        
    }
}

// Attributes in the form of ' "mTag_mId_"mPoint" ' - > Point_0_mPos
// struct Point
// {
//     string      mTag;
//     int         mId;
//     int         mPointNum; // won't be an attrib, just some useful field for betwwen opens and closes
//     //////////////////////
//     vector      mPos;
//     int         mIndex;
//     vector      mNearestPositions[];

    
//     string getTag()
//     {
//         if (mTag != "")
//         return mTag;
//         else
//         return "Point";
//     }

//     void setup(StructManager aManager; int aPointNum)
//     {
//         mTag = this -> getTag();
//         mPointNum = aManager -> addMember(mTag, aPointNum);

//         mPos = set(0.0, 0.0, 0.0);
//         aManager -> addAttrib(mTag, "mPos", mPos);
//         mIndex = 0;
//         aManager -> addAttrib(mTag, "mIndex", mIndex);
//         aManager -> addAttrib(mTag, "mNearestPositions", mNearestPositions);
//     }

//     void open(StructManager aManager; int aId)
//     {
//         mTag = this -> getTag();
//         mId = aId;
//         mPointNum = aManager -> getPointNumber(mTag, mId);

//         mPos = aManager -> getAttrib(mTag, "mPos", mPointNum);
//         mIndex = aManager -> getAttrib(mTag, "mIndex", mPointNum);
//         mNearestPositions = aManager -> getAttrib(mTag, "mNearestPositions", mPointNum);
//     }

//     void close(StructManager aManager)
//     {
//         aManager -> setAttrib(mTag, "mPos", mPointNum, mPos);
//         aManager -> setAttrib(mTag, "mIndex", mPointNum, mIndex);
//         aManager -> setAttrib(mTag, "mNearestPositions", mPointNum, mNearestPositions);
//     }
// }

// // Attributes in the form of ' "mTag_mId_"mPoint" ' - > EndPoint_2_mOrder
// struct EndPoint
// {
//     string      mTag;
//     int         mId;
//     int         mPointNum;
//     //////////////////////
//     Point       mPoint;
//     int         mOrder;
//     int         mIsOutside;
//     int         mOppositePoint;
//     float       mDirection;

//     string getTag()
//     {
//         if (mTag != "")
//         return mTag;
//         else
//         return "EndPoint";
//     }
    
//     void setup(StructManager aManager; int aPointNum)
//     {
//         mTag = this -> getTag();
//         mPointNum = aManager -> addMember(mTag, aPointNum);

//         mPoint.mTag = mTag + "_" + itoa(mId) + "_" + mPoint -> getTag();
//         mPoint -> setup(aManager, mPointNum);
//         mOrder = 0;
//         aManager -> addAttrib(mTag, "mOrder", mOrder);
//         mIsOutside = 0;
//         aManager -> addAttrib(mTag, "mIsOutside", mIsOutside);
//         mOppositePoint = -1;
//         aManager -> addAttrib(mTag, "mOppositePoint", mOppositePoint);
//         mDirection = 0;
//         aManager -> addAttrib(mTag, "mDirection", mDirection);
//     }

//     void open(StructManager aManager; int aId)
//     {
//         mTag = this -> getTag();
//         mId = aId;
//         mPointNum = aManager -> getPointNumber(mTag, mId);

//         mPoint.mTag = mTag + "_" + itoa(mId) + "_" + mPoint -> getTag();
//         mPoint -> open(aManager, 0);
//         mOrder = aManager -> getAttrib(mTag, "mOrder", mPointNum);
//         mIsOutside = aManager -> getAttrib(mTag, "mIsOutside", mPointNum);
//         mOppositePoint = aManager -> getAttrib(mTag, "mOppositePoint", mPointNum);
//         mDirection = aManager -> getAttrib(mTag, "mDirection", mPointNum);
//     }

//     void close(StructManager aManager)
//     {
        
//         mPoint -> close(aManager);
//         aManager -> setAttrib(mTag, "mOrder", mPointNum, mOrder);
//         aManager -> setAttrib(mTag, "mIsOutside", mPointNum, mIsOutside);
//         aManager -> setAttrib(mTag, "mOppositePoint", mPointNum, mOppositePoint);
//         aManager -> setAttrib(mTag, "mDirection", mPointNum, mDirection);
//     }
// }

#endif

