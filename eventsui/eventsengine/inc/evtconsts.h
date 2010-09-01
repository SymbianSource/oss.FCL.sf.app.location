/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Constants for project Storage API
*
*/


#ifndef EVTCONSTS_H
#define EVTCONSTS_H

//-- Database Names
_LIT(KEventsDbSecureFName, "c:[2001E663]EvtStorage.db");

//-- Config 
_LIT8(KServerConfigString, ";cache_size =  1024 ;page_size =1024 ;encoding =  \"UTF-8\" ");

//===============================
// Create Table:
//===============================
_LIT8(KCreateEventTable, "\
CREATE TABLE Event(\
EventId INTEGER,\
Status INTEGER,\
Subject VARCHAR(160) DEFAULT NULL,\
Place VARCHAR(160) DEFAULT NULL,\
Radius REAL,\
Description VARCHAR(160) DEFAULT NULL,\
Repeat BOOLEAN)\
");

_LIT8(KCreateActionTable, "\
CREATE TABLE Action(\
Id INTEGER PRIMARY KEY,\
Action VARCHAR(160) DEFAULT NULL,\
EvtId INTEGER)\
");
//===============================
// Indexes:
//===============================
_LIT8(KCreateEventStatusSubjectIndex, "CREATE INDEX EventStatus ON Event (Status)");
_LIT8(KCreateEventEvtIdIndex, "CREATE INDEX EventEvtID ON Event (EventId)");
_LIT8(KCreateActionEvtIdIndex, "CREATE INDEX ActionID ON Action (EvtId)");

//===============================
// Trigger: for Action and Place
//===============================
//-- Foreign Key Preventing insert
_LIT8(KCreateTriggerActionEventInsert, "\
CREATE TRIGGER fki_Action_EvtId_Event_EventId\
BEFORE INSERT ON [Action]\
FOR EACH ROW BEGIN\
  SELECT RAISE(ROLLBACK, 'insert on table \"Action\" violates foreign key constraint \"fki_Action_EvtId_Event_EventId\"')\
  WHERE NEW.EvtId IS NOT NULL AND (SELECT EventId FROM Event WHERE EventId = NEW.EvtId) IS NULL;\
END;\
");

//-- Foreign key preventing update
_LIT8(KCreateTriggerActionEventUpdate, "\
CREATE TRIGGER fku_Action_EvtId_Event_EventId\
BEFORE UPDATE ON [Action] \
FOR EACH ROW BEGIN\
    SELECT RAISE(ROLLBACK, 'update on table \"Action\" violates foreign key constraint \"fku_Action_EvtId_Event_EventId\"')\
      WHERE NEW.EvtId IS NOT NULL AND (SELECT EventId FROM Event WHERE EventId = NEW.EvtId) IS NULL;\
END;\
");

//-- Cascading Delete
_LIT8(KCreateTriggerActionEventDelete, "\
CREATE TRIGGER fkdc_Action_EvtId_Event_EventId\
BEFORE DELETE ON Event \
FOR EACH ROW BEGIN\
    DELETE FROM Action WHERE Action.EvtId = OLD.EventId;\
END;\
");

//-- Drop Trigger
_LIT8(KDropTriggerActionEventInsert, "\
DROP TRIGGER fki_Action_EvtId_Event_EventId;\
");
_LIT8(KDropTriggerActionEventUpdate, "\
DROP TRIGGER fku_Action_EvtId_Event_EventId;\
");
_LIT8(KDropTriggerActionEventDelete, "\
DROP TRIGGER fkdc_Action_EvtId_Event_EventId;\
");

//===============================
// Queries: 
//===============================

// Define Literals that are used in Query
_LIT(KId, ":IdVal");
_LIT(KEventId, ":EventIdVal");
_LIT(KStatus, ":StatusVal");
_LIT(KSubject, ":SubjectVal");
_LIT(KPlace, ":PlaceVal");
_LIT(KRadius, ":RadiusVal");
_LIT(KUnit, ":UnitVal");
_LIT(KDescription, ":DescVal");
_LIT(KRepeat, ":RepeatVal");
_LIT(KAction, ":ActionVal");
_LIT(KEvtId, ":EvtIdVal");

// Query to select Event based on LBT's event Id
_LIT8(KSelectEventFromEventId, "SELECT Event.Status,Event.Subject,Event.Place,Event.Radius,Event.Description,Event.Repeat,Action.Id,Action.Action,Action.EvtId FROM Event INNER JOIN Action ON (Event.EventId=:EventIdVal AND Event.EventId=Action.EvtId);");

// Query to select All Events - Only Basic Information
_LIT8(KSelectBasicEvent, "SELECT Event.EventId,Event.Status,Event.Subject,Event.Place,Event.Repeat FROM Event ORDER BY Status,Subject COLLATE CompareC3;");

// Query to select All Events based on Status - Only Basic Information
_LIT8(KSelectBasicEventOnStatus, "SELECT Event.EventId,Event.Status,Event.Subject,Event.Place,Event.Repeat FROM Event WHERE Event.Status=:StatusVal ORDER BY Event.Subject COLLATE CompareC3;");

// Query to select All Events based on Status
_LIT8(KSelectEventOnStatus, "SELECT Event.EventId,Event.Status,Event.Subject,Event.Place,Event.Radius,Event.Description,Event.Repeat,Action.Action,Action.Id,Action.EvtId FROM Event INNER JOIN Action ON (Event.EventId=Action.EvtId) WHERE Event.Status=:StatusVal ORDER BY Event.Subject COLLATE CompareC3;");

// Query to check the prsence of Event
_LIT8(KSelectEvent, "SELECT EventId FROM Event WHERE Event.EventId=:EventIdVal;");

// Query to select All Action based on Status
_LIT8(KSelectAction, "SELECT Id FROM Action WHERE Action.Id=:IdVal;");

// Query to select Action based on Event Id
_LIT8(KSelectActionFromEvent, "SELECT Id FROM Action WHERE Action.EvtId=:EvtIdVal;");

// Query to Insert an Event into Event Table
_LIT8(KInsertEvent, "INSERT INTO Event(EventId,Status,Subject,Place,Radius,Description,Repeat) VALUES(:EventIdVal,:StatusVal,:SubjectVal,:PlaceVal,:RadiusVal,:DescVal,:RepeatVal)");

// Query to Insert an Action into Action Table
_LIT8(KInsertAction, "INSERT INTO Action(Action,EvtId) VALUES(:ActionVal,:EvtIdVal)");

// Queries to Inserts using TSqlRowSetUtil
_LIT(KInsertEventRowDesc, "INSERT INTO Event(EventId) VALUES(%ld)"); // Not Used
_LIT(KInsertActionRowDesc, "INSERT INTO Action(EvtId) VALUES(%d)");
_LIT(KInsertEventDesc, "INSERT INTO Event(EventId,Status,Subject,Place,Radius,Description,Repeat) VALUES(%ld,%d,'%S','%S',%f,'%S',%d)");
_LIT(KInsertActionDesc, "INSERT INTO Action(Action,EvtId) VALUES('%S',%d)");

// Query to Update Event table based on Event/Trigger Id
_LIT8(KUpdateEvent, "UPDATE Event SET Status=:StatusVal,Subject=:SubjectVal,Place=:PlaceVal,Radius=:RadiusVal,Description=:DescVal,Repeat=:RepeatVal WHERE EventId=:EventIdVal");

// Query to Update Action table based on Event/Trigger Id
_LIT8(KUpdateAction, "UPDATE Action SET Action=:ActionVal,EvtId=:EvtIdVal WHERE Id=:IdVal");

// Query to Update Event status based on Event/Trigger Id
_LIT8(KUpdateEventStatus, "UPDATE Event SET Status=:StatusVal WHERE EventId=:EventIdVal");

// Query to Delete Action based on Event/Trigger Id
_LIT8(KDeleteAction, "DELETE FROM Action WHERE Action.EvtId=:EvtIdVal");

// Query to Delete Event based on Event/Trigger Id
_LIT8(KDeleteEvent, "DELETE FROM Event WHERE Event.EventId=:EventIdVal");

// -- New Strings for Delete Multiple
// OR String
_LIT8(KOr, " OR ");

// AND String
_LIT8(KAnd, " AND ");

// AND String
_LIT8(KWhere, " WHERE ");

// Action String Id
_LIT8(KMultiActionId, "Action.EvtId = ");

// Event String Id
_LIT8(KMultiEventId, "Event.EventId = ");

// Query to Delete Action based on Event/Trigger Id
_LIT8(KDeleteMultiAction, "DELETE FROM Action WHERE ");

// Query to Delete Event based on Event/Trigger Id
_LIT8(KDeleteMultiEvent, "DELETE FROM Event WHERE ");

// Query to Delete Action based on Event/Trigger Id
_LIT8(KUpdateMultiStatus, "UPDATE Event SET Status = ");

#endif      // EVTCONSTS_H

// End of File
