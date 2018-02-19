#include "./sensor.h"

Sensor::Sensor (): Sensor(0) {}

Sensor::Sensor (uint8_t max_size):size_(0), max_size_(0) {
    if (max_size == 0) {
        data_ = NULL;
        lengths_ = NULL;
    } else {
        changeSize(max_size_);
    }
}

void Sensor::changeSize (uint8_t new_max_size) {
    // First just add new arrays
    void** new_data_ = new void*[new_max_size];
    uint8_t* new_lengths_ = new uint8_t[new_max_size];
    for (uint8_t i = 0; i < new_max_size; i++) {
        new_data_[i] = NULL;
        new_lengths_[i] = 0;
    }
    // If there was old data, migrate the data
    if (max_size_ > 0) {
        for (uint8_t i = 0; i < (max_size_ < new_max_size ? max_size_ : new_max_size); i++) {
            new_data_[i] = data_[i];
            new_lengths_[i] = lengths_[i];
        }
        delete[] data_;
        delete[] lengths_;
    }
    // Commit transition
    max_size_ = new_max_size;
    data_ = new_data_;
    lengths_ = new_lengths_;
}

/**
 * Adds data to the internal data list.
 * @param location pointer to the variable casted as void*
 * @param length size of the variable.
 * @return: Index to the data.
 */
uint8_t Sensor::addData (void* location, uint8_t length) {
    // First check if the data array has ever been initialized.
    if (size_ == max_size_) changeSize(max_size_ + 1);
    // Insert data at last location
    data_[size_] = location;
    lengths_[size_] = length;
    // Incement size
    size_ ++;
    return size_ - 1;
}

uint8_t Sensor::getLength (uint8_t index) {
    if (index > size_) return 0;
    return lengths_[index];
}

uint8_t* Sensor::getData (uint8_t index) {
    if (index > size_) return NULL;
    return (uint8_t*) data_[index];
}

uint8_t Sensor::getSize () {
    return size_;
}

/**
 * Translating from register number to data pointer
 * ====REGISTER MAP====
 * 0: Not connected
 * 1: Sensor Type
 * 2: Data Size
 * 3: (Reserved)
 * 4+2*k: Data in the k_th entry
 * 4+2*k+1: Length of the k_th entry
 */
uint8_t* Sensor::getRegister(uint8_t mar) {
    if (mar == 1)
        return (uint8_t*) &type;
    else if (mar == 2)
        return &size_;
    else {
        uint8_t k = (mar - 4) / 2;
        if (mar % 2 == 1)
            return &lengths_[k];
        else return (uint8_t*) data_[k];
    }
    // Undefined behavior for undefined registers
    return &address;
}

uint8_t Sensor::getRegisterSize(uint8_t mar) {
    if (mar == 1)
        return sizeof(type);
    else if (mar == 2)
        return 1;
    else {
        uint8_t k = (mar - 4) / 2;
        if (mar % 2 == 1)
            return 1;
        else return lengths_[k];
    }
    return 1;
}