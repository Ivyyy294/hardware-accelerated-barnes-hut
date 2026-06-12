
#define DBUFFER_SIZE 512

groupshared uint db_fBaseIndex;
groupshared uint db_bBaseIndex;
groupshared uint db_fCount;
groupshared uint db_bCount;
groupshared uint db_buffer[DBUFFER_SIZE * 2];
    
void DB_Initialize()
{
    db_fBaseIndex = 0;
    db_bBaseIndex = DBUFFER_SIZE;
    db_fCount = 0;
    db_bCount = 0;
}
    
void DB_Swap()
{
    //Swap buffer index
    uint tmp = db_fBaseIndex;
    db_fBaseIndex = db_bBaseIndex;
    db_bBaseIndex = tmp;
        
    db_fCount = db_bCount;
    db_bCount = 0;
}
    
bool DB_Push(uint val)
{
    if (db_bCount >= DBUFFER_SIZE)
        return false;
        
    db_buffer[db_bBaseIndex + db_bCount++] = val;
    return true;
}
    
bool DB_InterlockedPush(uint val)
{    
    uint index = 0;
    InterlockedAdd(db_bCount, 1, index);
        
    if (index >= DBUFFER_SIZE)
    {
        // Undo increment
        InterlockedAdd(db_bCount, -1);
        return false;
    }
        
    db_buffer[db_bBaseIndex + index] = val;
    return true;
}
    
bool DB_Pop(inout uint val)
{
    if (db_fCount <= 0)
        return false;

    val = db_buffer[db_fBaseIndex + --db_fCount];
    return true;
}
    
bool DB_InterlockedPop(inout uint val)
{
    uint fCount = 0;
    InterlockedAdd(db_fCount, -1, fCount);
        
    if (fCount == 0 || fCount > DBUFFER_SIZE)
    {
        // Undo decrement
        InterlockedAdd(db_fCount, 1);
        return false;
    }
        
    uint index = fCount - 1;
    val = db_buffer[db_fBaseIndex + index];
    return true;
}
    
bool DB_Peek(in uint index, inout uint val)
{
    if (db_fCount <= 0 || index >= db_fCount)
        return false;
        
    val = db_buffer[db_fBaseIndex + index];
    return true;
}
    
bool DB_IsFull()
{
    return db_bCount >= DBUFFER_SIZE;
}
    
bool DB_IsFrontEmpty()
{
    return db_fCount <= 0;
}
    
bool DB_IsBackEmpty()
{
    return db_bCount <= 0;
}
