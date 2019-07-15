#pragma once 

/**
 * @file estring.h
 * @brief 
 * easy string with fixed length buffer
 **/

#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define estrncat(es,fmt,arg...) \
	if ((es).left() > 0) { \
		int __temp_len = snprintf((es).getBuffer()+(es).length(), (es).left(), fmt, ##arg); \
		if (__temp_len >= int ((es).left())) { \
			__temp_len = (es).left() - 1; \
		} \
		if (__temp_len >= 0) { \
			(es).setLength((es).length() + __temp_len); \
		} \
	}

/**
 * @brief 可变buffer长度的字符串类.Easy String
**/
class EString
{

public:

    /**
     * @brief 构造函数.
     *
     * @param [in] buffer 底部buffer地址.
     * @param [in] size 底部buffer大小.
     * @note 此构造函数不申请内存. 
	 * @warning 
	 **/
    EString(char * buffer, size_t size): m_size(size), m_isSelfBuf(false)
    {
        m_buffer = buffer;
		m_buffer[0] = '\0';
        m_length = 0;
	}

	/**
	 * @brief 构造函数.
	 *
	 * @param [in] size 底部buffer大小.
	 * @note 此构造函数会申请内存.
	 **/
	explicit EString(size_t size = 512): m_size(size), m_isSelfBuf(true)
	{
		m_buffer = (char *) malloc(size);
		assert(m_buffer != NULL);
		m_buffer[0] = '\0';
		m_length = 0;
	}

    ~EString()
	{
		if (m_isSelfBuf && m_buffer != NULL)
		{
			free(m_buffer);
			m_buffer = NULL;
		}
	}

    /**
     * @brief 更新数据.
     *
     * @return  void 
	 **/
    void refresh()
    {
        m_buffer[0] = '\0';
        m_length = 0;
    }

	/**
	 * @brief 重新分配内存，如果原有内存不够，则分配新的内存
	 * 只对内存是自身分配的情况才进行reserve()
	 * @warning 会释放旧内存，默认不会保留旧数据，请在使用前判断目标内存大小后再reserve()
	 * @param 需要保留旧数据，设置keep = true
	 **/
	bool reserve(size_t size, bool keep = false)
	{
		if (size < m_size)
		{
			return true;
		}

		if (!m_isSelfBuf)
		{
			return false;
		}
		
		size = size_t (size >= 4000000 ? (size + 1000000) : (size * 1.25f + 1));

		//slog_write(size>=1000000?LL_NOTICE:LL_DEBUG, "estring reserve size %lu", size);

		char * temp = (char *) malloc(size);
		if (temp == NULL)
		{
			//slog_write(LL_WARNING, "estring reserve malloc %lu failed %s", size, strerror(errno));
			return false;
		}

		if (keep && m_length > 0)
		{
			::memcpy(temp, m_buffer, m_length);
		}
		else
		{
			m_length = 0;
		}

		free(m_buffer);
		m_buffer = temp;
		m_buffer[m_length] = '\0';
		m_size = size;
		return true;
	}

    /**
     * @brief 获取底部buffer.
     *
     * @return  const char* 底部buffer位置.
	 **/
    const char * getBuffer() const { return m_buffer; }

    /**
     * @brief 获取底部buffer.
     *
     * @return  char*  底部buffer位置.
	 **/
    char * getBuffer() { return m_buffer; }

	/**
     * @brief 获取当前字符串有效长度.
     *
     * @return  size_t 当前字符串有效长度.
	 **/
    size_t length() const { return m_length; }

    /**
     * @brief 设置当前buffer中字符串有效长度.
     *
     * @param [in] len 有效长度.
     * @return  void 
	 **/
    void setLength(size_t len) { m_length = len; }

    /**
     * @brief 获取底部buffer大小.
     *
     * @return  size_t 底部buffer大小.
	 **/
    const size_t size() const { return m_size; }

	/**
	 * @brief 获取当前字符串剩余空间
	 **/
	const size_t left() const { return (m_size > m_length) ? (m_size - m_length) : 0; }

	/**
	 * @brief 从src拷贝n个字符, 若源串过长则截断
	 **/ 
	const char * strncpy(const char* src, size_t n)
	{
		if (m_size == 0)
		{
			return NULL;
		}

		if (n > m_size - 1)
		{
			n = m_size - 1;
		}

		char * pbuffer = m_buffer;
		::strncpy(pbuffer, src, n);

		pbuffer[n] = '\0';
		m_length = n;
    
		return pbuffer;
	}

	/**
	 * @brief 拷贝src, 假定src以\0结尾
	 **/ 
	const char * strcpy(const char * src)
	{
		return strncpy(src, strlen(src));
	}

	/**
	 * @breif 安全拷贝，假定src以\0结尾
	 **/
	const char * strscpy(const char * src)
	{
		size_t size = strlen(src);
		if (!reserve(size))
		{
			return NULL;
		}

		return strncpy(src, size);
	}

	/**
	 * @brief 安全strcat，假定src以\0结尾
	 * @param len not 0 mean strlen(src)
	 **/
	const char * strscat(const char * src, size_t len = 0)
	{
		if (src == NULL)
		{
			return NULL;
		}

		if (len == 0)
		{
			len = strlen(src);
		}

		if (!reserve(m_length+len, true))
		{
			return NULL;
		}

		::memcpy(m_buffer+m_length, src, len);
		m_buffer[m_length+len] = '\0';
		setLength(m_length+len);
		return m_buffer;
	}

    const char * memcpy(const char *src, size_t size)
    {
        if (!reserve(size))
        {
            return NULL;
        }

        ::memcpy(m_buffer, src, size);
		m_length = size;
        return m_buffer;
    }


private:

	EString(const EString &);

	EString & operator = (const EString &);


    char * m_buffer;		///< 底层buffer指针.
    size_t m_length;		///< 有效字符串长度.
	
	size_t m_size;	///< 底层buffer大小.
	const bool m_isSelfBuf;	///< 是否是类自身分配的buffer
};

