class CStack
{
    uint m_stack[512];
    int m_count;
    
    void Initialize()
    {
        m_count = 0;
    }
    
    bool Push (uint val)
    {
        if (m_count >= 512)
            return false;
        
        m_stack[m_count++] = val;
        return true;
    }
    
    bool Pop (inout uint val)
    {
        if (m_count <= 0)
            return false;

        val = m_stack[--m_count];
        return true;
    }
    
    bool IsEmpty()
    {
        return m_count <= 0;
    }
};