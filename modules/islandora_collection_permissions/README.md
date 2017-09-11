# Islandora Collection Permissions

## Introduction

Exposes some sets of permissions for configuration at the collection level.

## Requirements

This module requires the following modules/libraries:

* [Islandora Access Override](https://github.com/discoverygarden/islandora_access_override)

## Installation

Install as usual, see
[this](https://drupal.org/documentation/install/modules-themes/modules-7) for
further information.

## Usage

To account for the possiblity of membership in multiple collections, a breadth-
first traversal upwards from objects is used by default. In true tree
collection structures, this should act the same as a depth-first search. When
using the "Solr ancestor field" option, the selected subtree will be searched
depth-first, as the ancestor field does not track the node depth.

## Troubleshooting/Issues

This module mirrors core permissions and does not create new conventions. Since
in the Islandora permissions set to access the UI to purge an object one must
have permissions to manage its properties the same is true with the permission
set provided by this module.

Having problems or solved a problem? Contact
[discoverygarden](http://support.discoverygarden.ca).

## Maintainers/Sponsors

Current maintainers:

* [discoverygarden](http://www.discoverygarden.ca)

## Development

If you would like to contribute to this module, please check out our helpful
[Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers)
info, [Developers](http://islandora.ca/developers) section on Islandora.ca and
contact [discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
