
#define DBUFFER_SIZE 128

class CDoubleBuffer
{
    uint m_fBaseIndex;
    uint m_bBaseIndex;
    uint m_fCount;
    uint m_bCount;
    uint m_buffer[DBUFFER_SIZE * 2];
    
    void Initialize()
    {
        m_fBaseIndex = 0;
        m_bBaseIndex = DBUFFER_SIZE;
        m_fCount = 0;
        m_bCount = 0;
    }
    
    void Swap()
    {
        //Swap buffer index
        uint tmp = m_fBaseIndex;
        m_fBaseIndex = m_bBaseIndex;
        m_bBaseIndex = tmp;
        
        m_fCount = m_bCount;
        m_bCount = 0;
    }
    
    bool Push(uint val)
    {
        if (m_bCount >= DBUFFER_SIZE)
            return false;
        
        m_buffer[m_bBaseIndex + m_bCount++] = val;
        return true;
    }
    
    bool Pop(inout uint val)
    {
        if (m_fCount <= 0)
            return false;

        val = m_buffer[m_fBaseIndex + --m_fCount];
        return true;
    }
    
    bool IsFull()
    {
        return m_bCount >= DBUFFER_SIZE;
    }
    
    bool IsFrontEmpty()
    {
        return m_fCount <= 0;
    }
    
    bool IsBackEmpty()
    {
        return m_bCount <= 0;
    }
};