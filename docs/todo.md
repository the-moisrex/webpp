# TODOs

* [ ] Administrator commands
  * [ ] Add custom administrative commands
  * [ ] Add predefined administrative commands
    * [ ] Change configs
      * [ ] Inject/Change API KEY at runtime
    * [ ] Enable/Disable modules
    * [ ] Change module configs
      * [ ] Session manager module
      * [ ] User manager module
      * [ ] Database manager module
      


## Modules

* [ ] Session manager module
  * [ ] Inject/remove/change cookie for:
    * [ ] Everyone
    * [ ] Specific user
    * [ ] Specific session of a user
  * [ ] Clear sessions:
    * [ ] Clear all sessions
    * [ ] Clear specific user sessions
    * [ ] Clear specific user session
    * [ ] Clear sessions containing specific value
  * [ ] Add/Remove/Change key/value data to:
    * [ ] All sessions
    * [ ] Specific session
    * [ ] Sessions containing specific key/value
    * [ ] Specific user sessions
* [ ] User manager module (Uses Session and Cookie manager modules)
  * [ ] Logout a user from all session
  * [ ] Logout a user from a specific session
  * [ ] Disallow a device
  * [ ] Disallow all devices
  * [ ] Disable/Enable Login
  * [ ] Clear session data in all sessions or a specific session
  * [ ] Access to session modules to be able to modify those sessions
  * [ ] Authorization
  * [ ] Authentication
* [ ] Database manager modules
  * [ ] Add/Delete/Alter database
  * [ ] Query and stuff
  * [ ] Full ORM
  * [ ] Encrypt values
* [ ] Stats manager module
  * [ ] Parts:
    * [ ] All visits
    * [ ] IP based
    * [ ] Route visits
    * [ ] URI visits
    * [ ] User specific stats
    * [ ] Session specific stats
  * [ ] Storage types:
    * [ ] File
    * [ ] Memory
    * [ ] SQlite
    * [ ] Custom Database
  * [ ] Enable/Disable stat parts
  * [ ] What to save on each part:
    * [ ] IP
    * [ ] Headers:
      * [ ] User-Agent
      * [ ] Referer
      * [ ] All headers
      * [ ] Header signature
    * [ ] Device
    * [ ] Route
    * [ ] URI
    * [ ] Timestamp
  * [ ] User/session history (roadmap of session)
    * [ ] Previous stat record
    * [ ] Next stat record
    * [ ] All stat records for:
      * [ ] All sessions of a user
      * [ ] All sessions of all users
      * [ ] A specific session
  * [ ] Reports
    * [ ] Headers (percentage, count)
    * [ ] Devices (percentage, count)
    * [ ] Routes (percentage, count)
    * [ ] Search engines (percentages, count)
    * [ ] Referer (percentage, count)
  * [ ] Client tracker (JS needed)
    * [ ] Mouse moves
    * [ ] Mouse clicks
    * [ ] Keyboard clicks
    * [ ] Links hovered
    * [ ] Links clicked
    * [ ] Input intractions (filling forms, etc.)
    * [ ] Scrollbar intrests (where the users want to see more)
    * [ ] Haven't seen parts
* [ ] Device manager module
  * [ ] Device name
  * [ ] Device operating system
  * [ ] Device browser
  * [ ] Features in the browser
    * [ ] Does it have ....?


## Porting to other languages

### NodeJS
(NodeJS Doc on C++ AddOns)[https://nodejs.org/dist/latest-v8.x/docs/api/addons.html]




