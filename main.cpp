#include <memory>
#include <cstdint>
#include <cmath>
#include <limits.h>
#include <assert.h>
#include <iostream>

// g++-5 -g -std=c++1y main.cpp -o main -Wall -Wextra

class Hash {
    private:
        uint32_t T[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                               0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                               0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                               0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                               0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                               0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                               0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                               0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                               0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                               0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                               0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                               0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                               0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                               0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                               0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 
                               0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

        //Aux functions
    //   F(X,Y,Z) = XY v not(X) Z
    //   G(X,Y,Z) = XZ v Y not(Z)
    //   H(X,Y,Z) = X xor Y xor Z
    //   I(X,Y,Z) = Y xor (X v not(Z))
        inline uint32_t aux_F(uint32_t X, uint32_t Y, uint32_t Z) {
            return (X & Y) | ( (~X) & Z );
        }

        inline uint32_t aux_G(uint32_t X, uint32_t Y, uint32_t Z) {
            return (X & Z) | ( Y & (~Z));
        }

        inline uint32_t aux_H(uint32_t X, uint32_t Y, uint32_t Z) {
            return X ^ Y ^ Z;
        }

        inline uint32_t aux_I(uint32_t X, uint32_t Y, uint32_t Z) {
            return Y ^ (X | (~Z));
        }

        inline uint32_t rotl_32 (uint32_t n, unsigned int c) {
            const unsigned int mask = (CHAR_BIT*sizeof(n)-1);

            assert ( (c<=mask) &&"rotate by type width or more");
            c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
            return (n<<c) | (n>>( (-c)&mask ));
        }

        /* Round 1. */
        /* Let [abcd k s i] denote the operation
            a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
        inline uint32_t do_round_1(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X_k, size_t s, size_t i) {
            return B + rotl_32(A + aux_F(B,C,D) + X_k + T[i], s);
        }

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
        a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        inline uint32_t do_round_2(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X_k, size_t s, size_t i) {
            return B + rotl_32(A + aux_G(B,C,D) + X_k + T[i], s);
        }

        /* Round 3. */
        /* Let [abcd k s t] denote the operation
        a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        inline uint32_t do_round_3(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X_k, size_t s, size_t i) {
            return B + rotl_32(A + aux_H(B,C,D) + X_k + T[i], s);
        }

        /* Round 4. */
        /* Let [abcd k s t] denote the operation
        a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        inline uint32_t do_round_4(uint32_t A, uint32_t B, uint32_t C, uint32_t D, uint32_t X_k, size_t s, size_t i) {
            return B + rotl_32(A + aux_I(B,C,D) + X_k + T[i], s);
        }

    public:
        Hash() {
        }

        void hash(std::unique_ptr<uint32_t[]> data, size_t length);
};

void Hash::hash(std::unique_ptr<uint32_t[]> data, size_t length) {
    size_t padding_length = 16 - ((length + 3) % 16);
    std::cout << "Padding length: " << padding_length << std::endl;

    size_t padding_offset = length % 16;
    std::cout << "Padding offset: " << padding_offset << std::endl;
    std::unique_ptr<uint32_t[]> padding = std::make_unique<uint32_t[]>(16);

    for(int i=0; i < 16; ++i) {
        padding[i] = 0;
    }

    std::cout << "Length: " << length << std::endl;
    std::cout << "Copy start: " << (length & ~0xf) << std::endl;

    for(int i=0, j = (length & ~0xf); i < padding_offset; ++i, ++j) {
        padding[i] = data[j];
    }

    padding[padding_offset] = 0x0000'0080;
    padding[14] = (32*length) & 0xffff'ffff;
    padding[15] = (32*length) >> 32;

    std::cout << "Original: " << std::endl;
    for(int i=0; i < length; i += 1) {
        std::cout << std::hex << data[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Hashing: " << std::endl;
    for(int i=0; i < 16 * (int)(length/16); i += 1) {
        std::cout << std::hex << data[i] << " ";
    }

    for(int i=0; i < 16; ++i) {
        std::cout << std::hex << padding[i] << " ";
    }

    std::cout << std::endl;

    //init buffer
    //  word A: 01 23 45 67
    //   word B: 89 ab cd ef
    //   word C: fe dc ba 98
    //   word D: 76 54 32 10

    uint32_t A = 0x67452301;
    uint32_t B = 0xefcdab89;
    uint32_t C = 0x98badcfe;
    uint32_t D = 0x10325476;

    //do iters
    for(int i=0; i < 16 * (int)(length/16); i += 16) {
        size_t X_offset = i;

        uint32_t A_temp = A;
        uint32_t B_temp = B;
        uint32_t C_temp = C;
        uint32_t D_temp = D;

        //round 1
        for(int k=0; k < 4; ++k) {
            A = do_round_1(A, B, C, D, data[X_offset + ( (4*k) % 16 )], 7, 4*k+1 -1);
            D = do_round_1(D, A, B, C, data[X_offset + ( (4*k+1) % 16)], 12, 4*k+2 -1);
            C = do_round_1(C, D, A, B, data[X_offset + ( (4*k+2) % 16)], 17, 4*k+3 -1);
            B = do_round_1(B, C, D, A, data[X_offset + ( (4*k+3) % 16)], 22, 4*k+4 -1);
        }
        //round 2
        for(int k=0; k < 4; ++k) {
            A = do_round_2(A, B, C, D, data[X_offset + ( (4*k+1) % 16)], 5, 4*k+17 -1);
            D = do_round_2(D, A, B, C, data[X_offset + ( (4*k+6) % 16)], 9, 4*k+18 -1);
            C = do_round_2(C, D, A, B, data[X_offset + ( (4*k+11) % 16)], 14, 4*k+19 -1);
            B = do_round_2(B, C, D, A, data[X_offset + ( (4*k+16) % 16)], 20, 4*k+20 -1);
        }

        //round 3
        for(int k=0; k < 4; ++k) {
            A = do_round_3(A, B, C, D, data[X_offset + ( (12*k+5) % 16)], 4, 4*k+33 -1);
            D = do_round_3(D, A, B, C, data[X_offset + ( (12*k+8) % 16)], 11, 4*k+34 -1);
            C = do_round_3(C, D, A, B, data[X_offset + ( (12*k+11) % 16)], 16, 4*k+35 -1);
            B = do_round_3(B, C, D, A, data[X_offset + ( (12*k+14) % 16)], 23, 4*k+36 -1);
        }
        
        //round 4
        for(int k=0; k < 4; ++k) {
            A = do_round_4(A, B, C, D, data[X_offset + ( (12*k) % 16)], 6, 4*k+49 -1);
            D = do_round_4(D, A, B, C, data[X_offset + ( (12*k+7) % 16)], 10, 4*k+50 -1);
            C = do_round_4(C, D, A, B, data[X_offset + ( (12*k+14) % 16)], 15, 4*k+51 -1);
            B = do_round_4(B, C, D, A, data[X_offset + ( (12*k+5) % 16)], 21, 4*k+52 -1);
        }

        //update
        A += A_temp;
        B += B_temp;
        C += C_temp;
        D += D_temp;
    }

    //do padding
    for(int i=0; i < 16; i += 16) {
        uint16_t X_offset = i;

        uint32_t A_temp = A;
        uint32_t B_temp = B;
        uint32_t C_temp = C;
        uint32_t D_temp = D;

        //round 1
        for(int k=0; k < 4; ++k) {
            A = do_round_1(A, B, C, D, padding[X_offset + ( (4*k) % 16 )], 7, 4*k+1 -1);
            D = do_round_1(D, A, B, C, padding[X_offset + ( (4*k+1) % 16)], 12, 4*k+2 -1);
            C = do_round_1(C, D, A, B, padding[X_offset + ( (4*k+2) % 16)], 17, 4*k+3 -1);
            B = do_round_1(B, C, D, A, padding[X_offset + ( (4*k+3) % 16)], 22, 4*k+4 -1);
        }
        //round 2
        for(int k=0; k < 4; ++k) {
            A = do_round_2(A, B, C, D, padding[X_offset + ( (4*k+1) % 16)], 5, 4*k+17 -1);
            D = do_round_2(D, A, B, C, padding[X_offset + ( (4*k+6) % 16)], 9, 4*k+18 -1);
            C = do_round_2(C, D, A, B, padding[X_offset + ( (4*k+11) % 16)], 14, 4*k+19 -1);
            B = do_round_2(B, C, D, A, padding[X_offset + ( (4*k+16) % 16)], 20, 4*k+20 -1);
        }

        //round 3
        for(int k=0; k < 4; ++k) {
            A = do_round_3(A, B, C, D, padding[X_offset + ( (12*k+5) % 16)], 4, 4*k+33 -1);
            D = do_round_3(D, A, B, C, padding[X_offset + ( (12*k+8) % 16)], 11, 4*k+34 -1);
            C = do_round_3(C, D, A, B, padding[X_offset + ( (12*k+11) % 16)], 16, 4*k+35 -1);
            B = do_round_3(B, C, D, A, padding[X_offset + ( (12*k+14) % 16)], 23, 4*k+36 -1);
        }
        
        //round 4
        for(int k=0; k < 4; ++k) {
            A = do_round_4(A, B, C, D, padding[X_offset + ( (12*k) % 16)], 6, 4*k+49 -1);
            D = do_round_4(D, A, B, C, padding[X_offset + ( (12*k+7) % 16)], 10, 4*k+50 -1);
            C = do_round_4(C, D, A, B, padding[X_offset + ( (12*k+14) % 16)], 15, 4*k+51 -1);
            B = do_round_4(B, C, D, A, padding[X_offset + ( (12*k+5) % 16)], 21, 4*k+52 -1);
        }

        //update
        A += A_temp;
        B += B_temp;
        C += C_temp;
        D += D_temp;
    }

    std::cout << "A: " << std::hex << A << std::endl;
    std::cout << "B: " << std::hex << B << std::endl;
    std::cout << "C: " << std::hex << C << std::endl;
    std::cout << "D: " << std::hex << D << std::endl;
}

int main(int argc, char** argv) {
    Hash h;

    std::string s("The quick brown fox jumps over the lazy dog.");

    int padding = s.length() % 4;
    for(int i=0; i < padding; ++i) {
        s += '\0';
    }
    std::cout << "Initial string padding: " << padding << std::endl;

    int length = s.length() / 4;
    std::unique_ptr<uint32_t[]> data = std::make_unique<uint32_t[]>(length);

    for(int i=0; i < length; ++i) {
        data[i] = s[4*i] + (s[4*i + 1] << 8) + (s[4*i + 2] << 16) + (s[4*i+3] << 24);
    }

    auto ptr  = reinterpret_cast<uint8_t*>(data.get());
    for(int i =0 ; i < length*4; ++i) {
        std::cout << std::hex << (int)ptr[i] << " ";
    }
    std::cout  << std::endl;

    h.hash(std::move(data), length);
}