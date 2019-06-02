# TCP Message Encoder

A client and server that can encode and decode messages over TCP

## Usage

To compile run `make all`

## Project Details

Because this project requires no loss, no errors, and does not require a minimum bandwidth, TCP was chosen over UDP. Testing was done on macOS and Fedora Linux with ASCII text.  

## Extension of another Hash

This encoding has only one requirement of obfuscating the message. Not encrypting or making the message smaller. So the simplest hash that could be made is making an ASCII mapping table that converts to another psudeo-random character. The cons of this method are: it does not make the message any smaller, and it does not encrypt the message at all. 