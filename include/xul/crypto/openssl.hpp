#pragma once


namespace xul {


class aes_encrypter;
class aes_decrypter;


aes_encrypter* create_openssl_aes_cbc_encrypter();
aes_decrypter* create_openssl_aes_cbc_decrypter();


}
