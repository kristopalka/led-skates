#include <Arduino.h>

class Arg {
   private:
    char delimiter = '=';

   public:
    String key;
    String value;

    Arg(String key, String value) {
        this->key = key;
        this->value = value;
    }

    Arg(String arg) {
        this->key = get_value(arg, delimiter, 0);
        this->value = get_value(arg, delimiter, 1);
    }

    String toString() {
        return (key + delimiter + value).c_str();
    }

   private:
    String get_value(String data, char separator, int index) {
        int found = 0;
        int strIndex[] = {0, -1};
        int maxIndex = data.length() - 1;

        for (int i = 0; i <= maxIndex && found <= index; i++) {
            if (data.charAt(i) == separator || i == maxIndex) {
                found++;
                strIndex[0] = strIndex[1] + 1;
                strIndex[1] = (i == maxIndex) ? i + 1 : i;
            }
        }
        return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
    }
};