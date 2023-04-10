#ifdef ESP32
#include <esp_vfs.h>
#include <esp_vfs_dev.h>
#endif

extern "C" int main(int argc, char *argv[]);

void setup() {
    Serial.begin(115200);

#ifdef ESP32
    esp_vfs_dev_uart_use_driver(0);
#endif

    char *argv[] = {
        "ftinyduino", NULL};
    exit(main(1, argv));
}

void loop() {}

#ifdef TEENSYDUINO
    Print *stdPrint = &Serial;

extern "C" int _write(int file, const void *buf, size_t len) {
    Print *out;

    // Send both stdout and stderr to stdPrint
    if (file == stdout->_file || file == stderr->_file) {
        out = stdPrint;
    } else {
        out = (Print *)file;
    }

    if (out == nullptr) {
        return len;
    }

    // Don't check for len == 0 for returning early, in case there's side effects
    return out->write((const uint8_t *)buf, len);
}

Stream *stdStream = &Serial;

extern "C" int _read(int file, void *buf, size_t len) {
    Stream *in;

    if (file == stdin->_file) {
        in = stdStream;
    } else {
        in = (Stream *)file;
    }

    if (in == nullptr) {
        return 0;
    }

    static bool hasCR = false;

    char *b = (char *)buf;

    size_t count = 0;
    while (count < len) {
        // Note that readBytes is actually a timed read; it waits for input
        // See the Stream class for changing the timeout and
        // change as necessary on the object that stdStream points to
        char c;
        if (in->readBytes(&c, 1) == 0) {
            return count;
        }

        switch (c) {
        case '\r':
            hasCR = true;
            *(b++) = '\n';
            count++;
            return count;
        case '\n':
            if (!hasCR) {
                *(b++) = '\n';
                count++;
                return count;
            }
            // Skip this NL if it was preceded by a CR
            hasCR = false;
            break;
        default:
            hasCR = false;
            *(b++) = c;
            count++;
        }
    }

    return count;
}
#endif
