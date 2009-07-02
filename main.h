/*
 *  main.h
 *  XTagRev
 *
 *  Created by Kevin Ross on 07/02/09.
 *  Copyright 2009 uOttawa. All rights reserved.
 *
 */
#include <dbus-c++/dbus.h>
#include "server-glue.h"
class TagDBsrv : public com::ross::XTagRev::TagDB_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
TagDBsrv(Connection &connection);
void AddTag(const std::string& tagname);
void RmTag(const std::string& tagname);
vector< std::string > GetTagNames();
vector< std::string > GetTagSizes();
};