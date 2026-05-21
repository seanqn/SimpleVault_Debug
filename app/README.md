# SimpleVault

Secure local storage and encryption for credentials with a
simplified, user-friendly interface.

### DEBUG

## Authentication
Authentication flow intends to leverage the macOS/iOS
AuthenticationServices API (adopted from qtkeychain) which enables
passkey generation and authentication from
device methods such as PIN or biometrics.

This auth flow is still a work in progress and future versions may opt for an approach that doesn't require dependencies. Apple
SDK's 


Future versions may extend functionality to Windows.

***Elements marked * still need to be implemented***

### Auth Flow
1. Passkey Registration
    * Passkey Registration route depends on whether AES key
    has been created and stored in Keychain
    * ObjC class PasskeyBridge implements method startRegistration()
    that C++ class MacOSPasskeyServicer wraps and holds assigned
    ObjC class member as its own bridged C++ class member
    * Checks if AES key is stored in Keychain. If not, creates 
    new AES key, stores in Keychain, and call registration request
    via ObjC++ wrapper

2. Passkey Authentication
    * If an AES key is stored in keychain, call passkey authentication
    request. If not, create AES key and register passkey.
    * ObjC class PasskeyBridge impelements method startAuthentication()
    and is wrapped by MacOSPasskeyServicer

3. AES Key Retrieval
    * If successful authentication, retrieve the AES key from Keychain
    
4. Create Connection to QSqlLite Database
    * Allow read/write from database
    * Access is dependent on valid AES key retrieval, otherwise connection
    not established
    
#### Database Read/Write:
1. Read Entry
    * Get stored iv, secret, auth tag
    * Retrieve AES key
    * AES-GCM decrypt
    * If valid auth tag, return secret

2. Write Entry
    * Generate 96-bit IV
    * AES-GCM encrypt, requiring AES key
    * Encrypt plaintext, AES key, iv
    * Auth tag created by GCM
    * Push entry to database (base64 or blob)

** Development Note only **  
Libraries:
* Cryptopp (for AES key generation?)
* AuthenticationServices : Apple SDK (for passkey services)
* Security : Apple SDK (for Keychain)
