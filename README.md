# LEAP-Client-2023
LEAP Client is a software project that provides a digital interface for the Perkins Brailler, a mechanical device used for writing braille. It consists of two main components: a C++ program that runs on a Raspberry Pi Zero W, and a JavaScript server that runs on a separate device.

The C++ program interfaces with the Perkins Brailler, using a custom electronics assembly to convert keystrokes into a digital representation of a braille document. This digital representation is then sent to the JavaScript server via a network connection.

The JavaScript server receives the digital representation of the braille document, and converts it into a format that can be displayed on a website. This allows teachers who work with visually impaired students to easily view and provide feedback on the student's work.

## Usage
LEAP Client is designed to be used by teachers who are working with visually impaired students. The teacher should have a device that is connected to the same network as the Raspberry Pi Zero W, and should navigate to the server's URL in their web browser.

Once connected to the server, the teacher will be able to receive the digital representation of the braille document that the student has created on the Perkins Brailler. The teacher can then use this information to provide feedback to the student, or to make necessary changes to the document.

## Contributing
We welcome contributions from anyone who is interested in improving LEAP Client. If you would like to contribute, please follow these steps:

1. Fork this repository to your own account.

2. Create a new branch for your changes.

3. Make your changes and commit them.

4. Push your changes to your fork.

5. Open a pull request against this repository.

We will review your changes as soon as possible and merge them if they are in line with the goals of the project.

### License
LEAP Client is licensed under the MIT license. See the LICENSE file for more information.
