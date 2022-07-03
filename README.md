# Water Metering Solution 💦 - [Demonstration video][youtube] 👋 

[![backgroundImg](./img/background.png)][youtube]



## 🔭 Abstract

In this project we address a problem that touches parts of citizens about the correctness of their water bills. Many people complained that their bill does not match their consumption.  Some people published video clips that indicated that the water meter is counting while no water flows coming from the company water pipe, just air flow! Air can enter the water lines in many ways, and it can lead to a wrong reading in the homeowner’s water meter. Houses in higher land levels suffers more than houses in lower land levels. That motivated us to deeply study the problem. We came with the innovation of a custom sensor that measures both air and water flows and the percentage of each one. Using the custom sensor, one can numerically check whether there is an air flow in the pipe or not. As a proof of concept, we have designed and implemented an embedded system (based on ESP32) prototype for our custom sensor. Using water and air pumps, we additionally designed and implemented a physical system to mimic real home water lines with air flow.  We tested our system in different scenarios, such as: pure water flow, pure air flow, 50% air flow and other percentages. We found that our innovated meter can detect the percentages of water/air in the lines. In the real water lines supply, we noted that the air flow percentage is not constant. When pumping starts, the air flow (in a suffering house) is in its maximum level and start decaying until it becomes zero. Our final system comes with an electrical water valve and it is installed in the ground water tank of a house. The system periodically measures the air flow percentage in the water lines, if it is greater than certain threshold it closes the electrical water valve and then reopen at the start of a new period. 


### Connect with me:

[![website](./img/youtube-light.svg)][youtubeChannelW]
[![website](./img/youtube-dark.svg)][youtubeChannelD]
&nbsp;&nbsp;
[![website](./img/twitter-light.svg)][twitterW]
[![website](./img/twitter-dark.svg)][twitterD]
&nbsp;&nbsp;
[![website](./img/linkedin-light.svg)][linkedinW]
[![website](./img/linkedin-dark.svg)][linkedinD]


### Languages and Tools:

[<img align="left" alt="Visual Studio Code" width="26px" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/vscode/vscode-original.svg" style="padding-right:10px;" />][samepage]
[<img align="left" alt="JavaScript" width="26px" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/javascript/javascript-original.svg" style="padding-right:10px;" />][samepage]
[<img align="left" alt="CSS3" width="26px" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/css3/css3-original.svg" style="padding-right:10px;" />][samepage]
[<img align="left" alt="HTML5" width="26px" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/html5/html5-original.svg" style="padding-right:10px;" />][samepage]
[<img align="left" alt="ararduino d" width="26px" src="https://seeklogo.com/images/A/arduino-logo-BC7CBC1DAA-seeklogo.com.png" style="padding-right:10px;" />][samepage]
[<img align="left" alt="Git" width="26px" src="https://cdn.jsdelivr.net/gh/devicons/devicon/icons/git/git-original.svg" style="padding-right:10px;" />][samepage]
[<img align="left" alt="GitHub" width="26px" src="https://user-images.githubusercontent.com/3369400/139447912-e0f43f33-6d9f-45f8-be46-2df5bbc91289.png" style="padding-right:10px;" />][samepageD]
[<img align="left" alt="GitHub" width="26px" src="https://user-images.githubusercontent.com/3369400/139448065-39a229ba-4b06-434b-bc67-616e2ed80c8f.png" style="padding-right:10px;" />][samepageW]
[<img align="left" alt="Terminal" width="26px" src="./img/terminal-light.svg" />][samepageW]
[<img align="left" alt="Terminal" width="26px" src="./img/terminal-dark.svg" />][samepageD]

<br />
<br />

[youtube]: https://www.youtube.com/watch?v=ksbPfUVpSi8
[youtubeChannelW]: https://www.youtube.com/channel/UCVIea0-0IT5Hl7DjlnI-ssQ#gh-light-mode-only
[youtubeChannelD]: https://www.youtube.com/channel/UCVIea0-0IT5Hl7DjlnI-ssQ#gh-dark-mode-only
[twitterW]: https://twitter.com/aziiz_user#gh-light-mode-only
[twitterD]: https://twitter.com/aziiz_user#gh-dark-mode-only
[linkedinW]: https://www.linkedin.com/in/abdulaziz-alminqah-3354a522a#gh-light-mode-only
[linkedinD]: https://www.linkedin.com/in/abdulaziz-alminqah-3354a522a#gh-dark-mode-only
[samepage]: https://github.com/AziizMq/myProject
[samepageW]: https://github.com/AziizMq/myProject#gh-light-mode-only
[samepageD]: https://github.com/AziizMq/myProject#gh-dark-mode-only