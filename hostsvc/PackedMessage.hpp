#ifndef PACKEDMESSAGE_H
#define PACKEDMESSAGE_H

#include <memory>

using data_buffer = std::vector<char>;
const int HEADER_SIZE = 4;

template<typename T>
class PackedMessage {
public:
    PackedMessage() {
        m_msg = &m;
    }

    ~PackedMessage() {
    }

    void set_msg(const T *msg) {
        m_msg = const_cast<T *> (msg);
    }

    const T *get_msg() {
        return m_msg;
    }

    bool pack(data_buffer &buf) {
        if (!m_msg)
            return false;
        auto msg_size = m_msg->ByteSize();
        buf.resize(HEADER_SIZE + msg_size);
        encode_header(buf, msg_size);
        return m_msg->SerializeToArray(&buf[HEADER_SIZE], msg_size);
    }

    bool unpack(const data_buffer &buf) {

        return m_msg->ParseFromArray(buf.data() + HEADER_SIZE, (int)buf.size() - HEADER_SIZE);

    }

    static void encode_header(data_buffer &buf, unsigned size) {
        std::cout << size << " packed" << std::endl;
        assert(buf.size() >= HEADER_SIZE);
        buf[0] = static_cast<char>(size >> 24 & 0xFF);
        buf[1] = static_cast<char>(size >> 16 & 0xFF);
        buf[2] = static_cast<char>(size >> 8 & 0xFF);
        buf[3] = static_cast<char>(size & 0xFF);
    }

    static unsigned decode_header(const data_buffer &buf) {
        if (buf.size() < HEADER_SIZE)
            return 0;
        unsigned size = 0;
        for (auto i = 0; i < HEADER_SIZE; ++i) {
            auto t = static_cast<unsigned>(buf[i]) & 0xff;
            size = size * 256 + t;
        }

        std::cout << size << " unpacked" << std::endl;
        return size;
    }

    void setClientID(int x)
    {
        clientID=x;
    }
    void getClinetID()
    {
        return clientID;
    }


private:
    T *m_msg;
    T m;
    int clientID;
};

#endif 

