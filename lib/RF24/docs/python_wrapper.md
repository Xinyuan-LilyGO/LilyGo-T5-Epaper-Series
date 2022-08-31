# Python Wrapper

<!-- markdownlint-disable MD031 -->
By [mz-fuzzy](https://github.com/mz-fuzzy)

## Python Wrapper Prerequisites

### RF24

The RF24 lib needs to be built in C++ & installed for the python wrapper to wrap it.

See [Linux Installation](md_docs_linux_install.html) (or [installing with CMake](md_docs_using_cmake.html)
alternatively) and [Linux/RPi General](md_docs_rpi_general.html)

### Python2

```shell
sudo apt-get install python-dev libboost-python-dev python-pip python-rpi.gpio
```

Next, install some up-to-date python packages.

```shell
python -m pip install --upgrade pip setuptools
```

### Python3

```shell
sudo apt-get install python3-dev libboost-python-dev python3-pip python3-rpi.gpio
```

Next, install some up-to-date python3 packages.

```shell
python3 -m pip install --upgrade pip setuptools
```

## Installation

@note Steps 2 and 3 have to be repeated if installing the python wrappers for
RF24Network and RF24Mesh libraries. The prerequisites stated above still apply
to each library.

1. For python3, setup.py needs a manually created symlink for the boost.python library:
   ```shell
   sudo ln -s $(ls /usr/lib/$(ls /usr/lib/gcc | tail -1)/libboost_python3*.so | tail -1) /usr/lib/$(ls /usr/lib/gcc | tail -1)/libboost_python3.so
   ```
2. Build the library.

   This step and the next step need to be executed from the appropriate directory of
   the cloned RF24* repository:
   - navigate to *pyRF24* directory in the RF24 cloned repository
   - navigate to *RPi/pyRF24Network* directory in the RF24Network cloned repository
   - navigate to *pyRF24Mesh* directory in the RF24Mesh cloned repository

   When in the correct directory, run the following command:
   ```shell
   ./setup.py build
   ```
   or for python3
   ```shell
   python3 setup.py build
   ```
   @note Build takes several minutes on arm-based machines. Machines with RAM less than 1GB may need to increase amount of swap for build.
3. Install the library
   ```shell
   sudo ./setup.py install
   ```
   or for python3
   ```shell
   sudo python3 setup.py install
   ```
   See the additional [Platform Support pages](pages.html) for information on connecting your hardware.

   See the included [\*.py files in the "examples_linux" folder](examples.html) for usage information.
4. Running the Example

   The python examples location differ for each RF24* resopitories.
   - navigate to *examples_linux* directory in the RF24 cloned repository
   - navigate to *RPi/pyRF24Network/examples* directory in the RF24Network cloned repository
   - navigate to *examples_RPi* directory in the RF24Mesh cloned repository

   Navigate to the examples_linux directory
   ```shell
   cd ../examples_linux
   ```
   Edit the getting_started.py example to configure the appropriate pins per the [Linux/RPi General](md_docs_rpi_general.html) documentation.
   ```shell
   nano getting_started.py
   ```
   Configure another device, Arduino or RPi with the same example code. It could be C++ or python (see the [list of available examples](examples.html)), but we'll use the same example file on a different device in this tutorial.

   Run the example
   ```shell
   sudo python getting_started.py
   ```
   or for python3
   ```shell
   sudo python3 getting_started.py
   ```
