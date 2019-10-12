//
// Created by denn nevera on 2019-07-19.
//

#pragma once

#include <vector>

/***
 * Blowfish PKCS7
 */
class Blowfish {

public:

    using KeyType = std::vector<std::uint8_t>;
    using DataType = std::vector<std::uint8_t>;

public:

    /**
     * Create blowfish cipher with key
     * @param key
     */
    Blowfish(const KeyType key);
    ~Blowfish();

    /**
     * Encrypt data from to
     * @param input - clear input data
     * @param output - encrypted output
     */
    void encrypt(const DataType& input, DataType& output);

    /***
     * Decrypt data from to
     * @param input - encrypted input data
     * @param output - lear output data
     */
    void decrypt(const DataType& input, DataType& output);

private:

    KeyType key_;
    size_t N_;
    size_t block_size_;
    std::vector<std::uint32_t> P_;
    std::vector<std::vector<std::uint32_t>> S_;
    DataType iv_;
    static std::vector<std::uint32_t> origP_;
    static std::vector<std::vector<std::uint32_t>> origS_;
    Blowfish::DataType encrypt_prev_;
    Blowfish::DataType decrypt_prev_;

    void expandKey(const KeyType& key);

    void encryptBlowfishBlock(std::uint32_t &l, std::uint32_t &r);
    void decryptBlowfishBlock(std::uint32_t &l, std::uint32_t &r);

    DataType encryptSlice(const DataType& input);
    DataType decryptSlice(const DataType& input);

    inline std::uint32_t __F__(std::uint32_t x)  {
      auto t = S_[0][((x) >> 24) & 0xff];
      t += S_[1][((x) >> 16) & 0xff];
      t ^= S_[2][((x) >> 8) & 0xff];
      t += S_[3][(x) & 0xff];
      return t;
    }
};
