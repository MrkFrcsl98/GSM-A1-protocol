#include <iostream>
#include <string>
#include <vector>

std::vector<int> hexToBinary(const std::string &hex, int bits) {
  std::vector<int> binary(bits);
  for (int i = 0; i < bits; ++i) {
    binary[i] = (hex[i / 4] >> (3 - (i % 4))) & 1; // Extract each bit from hex
  }
  return binary;
}

void initializeRegisters(const std::vector<int> &key, std::vector<int> &R1, std::vector<int> &R2,
                         std::vector<int> &R3) {
  for (int i = 0; i < R1.size(); ++i) {
    R1[i] = key[i % key.size()];
  }
  for (int i = 0; i < R2.size(); ++i) {
    R2[i] = key[(i + R1.size()) % key.size()];
  }
  for (int i = 0; i < R3.size(); ++i) {
    R3[i] = key[(i + R1.size() + R2.size()) % key.size()];
  }
}

void generateCiphertext(const std::string &plaintext, const std::vector<int> &R1,
                        const std::vector<int> &R2, const std::vector<int> &R3,
                        std::string &ciphertext, std::string &keystream) {
  std::vector<int> R1_copy = R1;
  std::vector<int> R2_copy = R2;
  std::vector<int> R3_copy = R3;

  for (std::size_t i = 0; i < plaintext.length(); ++i) {
    int majority = (R1_copy.back() + R2_copy.back() + R3_copy.back()) > 1 ? 1 : 0;

    int newR1 = R1_copy[0] ^ majority;
    int newR2 = R2_copy[0] ^ majority;
    int newR3 = R3_copy[0] ^ majority;

    for (int j = 0; j < R1_copy.size() - 1; ++j)
      R1_copy[j] = R1_copy[j + 1];
    for (int j = 0; j < R2_copy.size() - 1; ++j)
      R2_copy[j] = R2_copy[j + 1];
    for (int j = 0; j < R3_copy.size() - 1; ++j)
      R3_copy[j] = R3_copy[j + 1];

    R1_copy.back() = newR1;
    R2_copy.back() = newR2;
    R3_copy.back() = newR3;

    int keystreamBit = (R1_copy.back() ^ R2_copy.back() ^ R3_copy.back());
    keystream += std::to_string(keystreamBit);

    int plaintextBit = plaintext[i] - '0';
    int ciphertextBit = plaintextBit ^ keystreamBit;
    ciphertext += std::to_string(ciphertextBit);
  }
}

// Function to decrypt ciphertext
std::string decryptCiphertext(const std::string &ciphertext, const std::string &keystream) {
  std::string decryptedText;
  for (std::size_t i = 0; i < ciphertext.length(); ++i) {
    int ciphertextBit = ciphertext[i] - '0';
    int decryptedBit = ciphertextBit ^ (keystream[i] - '0');
    decryptedText += std::to_string(decryptedBit);
  }
  return decryptedText;
}

int main() {
  std::string hexKey("123456789ABCDEF"), hexFrame("1A2B3C"), plaintext("10101010"), ciphertext,
      keystream;

  std::vector<int> key = hexToBinary(hexKey, 64);
  std::vector<int> frameNumber = hexToBinary(hexFrame, 22);
  std::vector<int> R1(19, 0), R2(22, 0), R3(23, 0);

  initializeRegisters(key, R1, R2, R3);

  generateCiphertext(plaintext, R1, R2, R3, ciphertext, keystream);

  std::cout << "Original  : " << plaintext << "\n";
  std::cout << "Keystream : " << keystream << "\n";
  std::cout << "Ciphertext: " << ciphertext << "\n";
  std::string decryptedText = decryptCiphertext(ciphertext, keystream);
  std::cout << "Decrypted : " << decryptedText << "\n";

  return 0;
}
