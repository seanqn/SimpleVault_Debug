# SimpleVault

Secure local storage and encryption for credentials with a
simplified, user-friendly interface.
####

### Debug - App is a work in progress

### Authentication Prototype
Uses macOS keychain via qtkeychain. A single master password is input on first build that future sign-ins will use. UID is tied
to the machine when the app is first built; changing master password is not supported at the moment. Future versions will likely aim
for a auth flow that is purely attached to the interface instead of depending on keychain. 

qtkeychain handles bridging between keychain and app. Backend auth logic which implements qtkeychain can be found in /services.

### Vault/Home/Groups Prototype
The home view serves as the anchor for rest of the UI after successful authentication. A Groups sidebar allows custom groups or
"vaults" to be added or removed.

### Future Additions
New features to be added will be built on the previously listed core features. Vault content will display clean and organized
lines or boxes of text that make it easy to identify usernames, passwords, organization names, and other credentials. An account
page is a necessary inclusion that will reflect the user's information and preferences.
