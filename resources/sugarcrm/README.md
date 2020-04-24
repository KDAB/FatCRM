# Sugar-Resource: a KDSoap based Akonadi Resource for SugarCRM's webservice interface

Synchronizes the collection tree at login, collection contents can be synced in clients
on demand.

Creates one top level collection which just contains further collections, one for each module.
Operations on each such collection are handled by a module/type specific `ModuleHandler` subclass.

## Dependencies

SOAP input file: sugar.wsdl (downloaded from http://$SUGARCRMHOST/crm/soap.php?wsdl)

Needs KDSoap installed for build and runtime:
  https://github.com/KDAB/KDSoap

## Debugging
==============

Set `KDSOAP_DEBUG` to 1 in the environment Akonadi is started from, e.g.

```
export KDSOAP_DEBUG=1
akonadictl start
```
