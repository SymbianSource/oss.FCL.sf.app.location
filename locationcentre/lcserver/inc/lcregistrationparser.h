/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location Centre Server object.
*
*/


#ifndef C_LCREGISTRATIONPARSER_H
#define C_LCREGISTRATIONPARSER_H

// SYSTEM INCLUDES
#include <Xml/Parser.h>
#include <Xml/ContentHandler.h>
#include <Xml/documentparameters.h>
#include <f32file.h>

//USER INCLUDE
#include "lcregxmltagtype.h"


// DATA TYPES
enum TXmlParserElementState
    {
    EXmlParserOnStartElement,
    EXmlParserOnEndElement
    };

// FORWARD DECLARATIONS
class CLcRegAppInfo;

/**
 *  class CLcRegistrationParser.
 *
 *  This class is inherited from CBase. This
 *	class is used as a parser for parsing the Location Centre Registration file.
 *  @since S60 v5.0
 */
class CLcRegistrationParser : public CBase, public Xml::MContentHandler
	{
public:
    /**
     * Constructs a new instance of CLcRegistrationParser.
     *
     * @return The new instance of CLcRegistrationParser object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegistrationParser* NewL( RFs& aFs );

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server registry handler
     */
    ~CLcRegistrationParser();

public: // New functions
    /**
     * create and initialize the parser.
     * 
     * @param aFile Filename to be parsed
     *   index.
     * @return returns a CLcRegAppInfo* object.
     */    
    CLcRegAppInfo* ParseFileL( const TDesC& aFile );

public: // Functions from base class MContentHandler

    /**
     * From From Xml::MContentHandler.
     *
     * @param aDocParam Document parameters.
     * @param aErrorCode A error code.
     */
    void OnStartDocumentL(
    /* IN */    const Xml::RDocumentParameters& aDocParam,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aErrorCode A error code.
     */
    void OnEndDocumentL(TInt aErrorCode);

    /**
     * From From Xml::MContentHandler.
     *
     * @param aElement Tag information.
     * @param aAttributes Attributes.
     * @param aErrorCode A error code.
     */
    void OnStartElementL(
    /* IN */   const Xml::RTagInfo& aElement,
    /* IN */   const Xml::RAttributeArray& aAttributes,
    /* IN */   TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aElement Tag information.
     * @param aErrorCode A error code.
     */
    void OnEndElementL(
    /* IN */   const Xml::RTagInfo& aElement,
    /* IN */   TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aBytes Tag content.
     * @param aErrorCode A error code.
     */
    void OnContentL(
    /* IN */   const TDesC8& aBytes,
    /* IN */   TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aPrefix A prefix.
     * @param aUri A Uri.
     * @param aErrorCode A error code.
     */
    void OnStartPrefixMappingL(
    /* IN */    const RString& aPrefix,
    /* IN */    const RString& aUri,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aPrefix A prefix.
     * @param aErrorCode A error code.
     */
    void OnEndPrefixMappingL(
    /* IN */    const RString& aPrefix,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aBytes A bytes.
     * @param aErrorCode A error code.
     */
    void OnIgnorableWhiteSpaceL(
    /* IN */    const TDesC8& aBytes,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aName A skipped entity.
     * @param aErrorCode A error code.
     */
    void OnSkippedEntityL(
    /* IN */    const RString& aName,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     * @param aTarget A target.
     * @param aData A data.
     * @param aErrorCode A error code.
     */
    void OnProcessingInstructionL(
    /* IN */    const TDesC8& aTarget,
    /* IN */    const TDesC8& aData,
    /* IN */    TInt aErrorCode
    );

    /**
     * From From Xml::MContentHandler.
     *
     */
    void OnOutOfData();

    /**
     * From From Xml::MContentHandler.
     *
     * @param aErrorCode A error code.
     */
    void OnError(/* IN */ TInt aErrorCode);

    /**
     * From From Xml::MContentHandler.
     *
     * @param aUid A Uid of extended interface.
     */
    TAny* GetExtendedInterface(/* IN */ const TInt32 aUid);

private:
    /**
     * C++ Default constructor
     */
    CLcRegistrationParser( RFs& aFs );
    

private:
    /**
     * Reads the attribute value for related elements.
     *
     * @param aElement Tag information.
     * @param aAttributes Attributes.
     */
	void CollectAttributeValueL(
	    const Xml::RTagInfo& aElement,
	    const Xml::RAttributeArray& aAttributes);

    /**
     * Sets read content.
     *
     * @param aTag Tag to set content for.
     * @param aContent Content to set.
     */
    void SetContentL(
        TLcRegXmlTagType aTag,
        const TDesC& aContent
    );
    
   /**
    * Set a parsed tag.
    *
    * @param aTag Tag to set parsed state for.
    */
    void SetParserTagL(TLcRegXmlTagType aTag);

   /**
    * Check that sequence order is correct.
    *
    * @param aTag Tag to check it is in right order.
    */
    void CheckSequenceOrderL(TLcRegXmlTagType aTag);

   /**
    * Check mandatory tag are correct.
    *
    * @param aXmlState This contains wheather 
    *				   parser state is in OnStartelement / OnEndElement
    * @param aTag Tag to check it is in right order.
    */
    void CheckMandatoryTagsL(
        TXmlParserElementState aXmlState,
        TLcRegXmlTagType aTag
    );
    
   /**
    * This function takes the Java midletname as input
    * parameter and returns the application Uid by which
    * this can be validate through app shell.
    *
    * @param aMidletName Contains the Java application midletname.
    * @return It returns the corresponding application uid
    *		  assigned to this midletname.
    */
	TUint32 JavaAppUidL( const TDesC&	aMidletName );

   /**
    * This function checks the attribute name
    * associated with the corresponding element
    * and if it is not valid one leaves with KErrNotSupported.
    * Also checks no of attributs should be present with the element.
    *
    * @param aElement    Contain the element name.
    * @param aAttributes Holds the attribute array
    *
    * @return It returns the valid attribute type
    */
	TLcRegXmlTagType CheckAndReturnValidAttrL(
					TLcRegXmlTagType aElement,
					const Xml::RAttributeArray& aAttributes );
	
        
    // By default, prohibit copy constructor
    CLcRegistrationParser( const CLcRegistrationParser& );
    // Prohibit assigment operator
    CLcRegistrationParser& operator= ( const CLcRegistrationParser& );

private: //data member
    //own: Application registration info
	CLcRegAppInfo*					iRegAppInfo;
	
	// File server reference from registry
	RFs&                             iFs;

    // Parser stuff
    HBufC8*                         iReadContent;
	
    // Input stuff
    TUint32							iParsedTag;
    TUint32							iParsingTag;
    RArray<TLcRegXmlTagType>        iParserTreeTags;
    
    //Own: used to copy all the attribute value of an element
	HBufC* 							iAttributeBuf;
	
	// Holds the registration file full path name
	TFileName						iFileName;
	
	// Holds the parser framework error status
	TInt							iParserError;    
	};

#endif // C_LCREGISTRATIONPARSER_H