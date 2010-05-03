#
# Copyright (c) 2010  Nokia Corporation and/or its subsidiary(-ies).


#
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description:
#
#


TEMPLATE = subdirs
SUBDIRS = maptileservice

# Build.inf rules
BLD_INF_RULES.prj_exports += "./mylocationsengine/loc/mylocations.loc   APP_LAYER_LOC_EXPORT_PATH(mylocations.loc)" 

BLD_INF_RULES.prj_mmpfiles = "./geocodeupdate/group/geocodeupdate.mmp" \
                             "./mylocationsengine/group/mylocationsengine.mmp"
# Exports
deploy.path = /

# IBY files
iby.path = epoc32/rom/include/core/app
iby.sources = mylocations.iby

for(iby, iby.sources):BLD_INF_RULES.prj_exports += "./rom/$$iby $$deploy.path$$iby.path/$$iby"


