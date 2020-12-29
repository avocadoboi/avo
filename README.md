# AvoGUI

AvoGUI is a light, modern framework for desktop applications written in c++20.

The project was started because I wanted a graphical framework designed from the user's perspective. The goal is a modern design that allows creative coding and interactive programs yet is scalable for writing larger applications. The API should be intuitive with documentation that explains things clearly and doesn't assume too much knowledge.

The project was inspired by Google's Material Design guidelines and includes some Material Design controls to help you make good looking applications quickly.

Warning: the project is still in development and a lot of breaking changes are being made.

## CMake usage
TODO: write about CMake integration.

## Getting started

Take a look at the documentation in AvoGUI.hpp for more information about anything. If there's something you wonder about not addressed in the documentation, please open an issue! More guides will also be made at some point, when the API has gotten somewhat stable. Meanwhile, the programs in the examples directory can be useful for learning about how to use the framework. They should be up to date with the API.

### Unicode characters

AvoGUI supports UTF-8 unicode characters, and all strings are assumed to be encoded in UTF-8. However note that UTF-8 is backwards-compatible with ASCII.

To support right-to-left languages, set the reading direction of the text to ReadingDirection::RightToLeft. You can set the default reading direction by modifying the default TextProperties in the DrawingContext.

## Compatible platforms

The goal is to support Windows, Linux and MacOS. Currently, only Windows is fully supported and Linux development is in progress.

## Contributing

Don't be afraid to submit issues about the little things! All issues are very welcome, and they help the development of the library a lot. Pull requests are also welcome.
