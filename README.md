# _mushin_ user interface for Amiga

This is _mushin_ - a MUI-compatible user interface library based on AROS Zune

## [amigazen project](http://www.amigazen.com)

*A web, suddenly*

*Forty years meditation*

*Minds awaken, free*

**amigazen project** uses modern software development tools and methods to update and rerelease classic Amiga open source software. Releases include a new AWeb, a new Amiga Python 2, and the ToolKit project - a universal SDK for Amiga.

Key to the amigazen project approach is ensuring every project can be built with the same common set of development tools and configurations, so the ToolKit project was created to provide a standard configuration for Amiga development. All *amigazen project* releases will be guaranteed to build against the ToolKit standard so that anyone can download and begin contributing straightaway without having to tailor the toolchain for their own setup.

The amigazen project philosophy is based on openness:

*Open* to anyone and everyone	- *Open* source and free for all	- *Open* your mind and create!

PRs for all of amigazen projects are gratefully received at [GitHub](https://github.com/amigazen/). While the focus now is on classic 68k software, it is intended that all amigazen project releases can be ported to other Amiga-like systems including AROS and MorphOS where feasible.

The original authors of the *AROS Zune* software are not affiliated with the amigazen project. This software is redistributed on terms described in the documentation, particularly the file LICENSE.md

## About _mushin_

_mushin_ is a free and open source user interface library for building Amiga applications, implementing the same APIs as MUI, the Magic User Interface, and based on code from AROS Zune, designed to be a drop in replacement for MUI. 

## About ToolKit

**ToolKit** exists to solve the problem that most Amiga software was written in the 1980s and 90s, by individuals working alone, each with their own preferred setup for where their dev tools are run from, where their include files, static libs and other toolchain artifacts could be found, which versions they used and which custom modifications they made. Open source collaboration did not exist as we know it in 2025. 

**ToolKit** from amigazen project is a work in progress to make a standardised installation of not just the Native Developer Kit, but the compilers, build tools and third party components needed to be able to consistently build projects in collaboration with others, without each contributor having to change build files to work with their particular toolchain configuration. 

All *amigazen project* releases will release in a ready to build configuration according to the ToolKit standard.

Each component of **ToolKit** is open source and will have it's own github repo, while ToolKit itself will eventually be released as an easy to install package containing the redistributable components, as well as scripts to easily install the parts that are not freely redistributable from archive.

## Building _mushin_

_mushin_ is designed to build against the ToolKit standard. The build process uses SAS/C with smake for Amiga, ensuring compatibility with classic Amiga development workflows.

Detailed build instructions will be available in the [BUILD.md](BUILD.md) file.

### Prerequisites

- SAS/C compiler (primary target)
- ToolKit development environment
- Amiga operating system 3.1 or higher

## Frequently Asked Questions

### Why the name _mushin_ ?

_mushin_ (無心) might be familiar to followers of martial arts or zen buddhism as a concept meaning “no-mind” — a state of effortless awareness where action flows naturally without conscious thought or hesitation. In this state, the mind is clear, responsive, and unhindered by distraction or intent. In english, a related phrase might be "muscle memory" or simply "instinct".

Therefore _mushin_ perfectly expresses what a good user interface should be. An interface should disappear when it’s doing its job — letting the user act fluidly, without friction or mental clutter. 

As the user interface should be effortless for the user, so should building the interface be effortless for the developer too. Developers familiar with the MUI API will therefore be able to make use of their existing 'muscle memory'.

There's more wordplay here too - on its own another homophone of the japanese _shin_ means 'new' and this is a 'new' implementation of the classic MUI API, although strictly speaking _mushin_ would then mean 'nothing new', which seems apt given the state of MUI development over the past 30 years.

Using the name _mui_ itself was considered, given that in Japanese the word 無意 (mu-i) literally means “without intention”. In zen buddhism, 無意 refers to the state of effortless action, similar to the Chinese wu wei (無為) in Daoism, however while the MUI product is long since abandoned, out of respect a new name is more appropriate. It could perhaps also be said that the SASG team have made 'no effort' these past 30 years, even their website is unchanged all this time.

### How compatible is _mushin_ with the original MUI?

To be determined.

### What is the relationship between _mushin_ and AROS Zune?

_mushin_ is forked from the October 2025 GitHub head of AROS, specifically only the Zune components. Forked, because the AROS repository maintainers do not intend to maintain compatibility with anything other than AROS, as evidenced by the disregard shown for Amiga and MorphOS compatibility in the AROS commit history for Zune. Still, there was an existing smakefile and much of the Zune code remains untouched since 2006 when it was still possible to build the Amiga target. Additionally the AfA OS project had an Amiga build of Zune that worked, and the sourecode of that has been cross referenced in rebuilding this Amiga-native version.

The project directory structure retains the AROS hierarchy making it easier to merge changes in either direction in the future.

### How does _mushin_ differ from the original MUI?

MUI had a reputation for complexity, often running slowly and using large amounts of memory on all but the most high end Amiga machines. It was also infinitely configurable, every part of the interface customisable, and also extensible with third party components. 

_mushin_ on the other hand is intended to be pared back, designed to be a more lightweight implementation, with intentionally fewer configurable options, while also being a more complete package, including a full set of third party components where source code is available.

### Which applications have been tested with _mushin_?

- Voyager
- iBrowse

## Contact 

- At GitHub https://github.com/amigazen/mushin/ 
- on the web at http://www.amigazen.com/ (Amiga browser compatible)
- or email toolkit@amigazen.com

## Acknowledgements

*Amiga* is a trademark of **Amiga Inc**. 
