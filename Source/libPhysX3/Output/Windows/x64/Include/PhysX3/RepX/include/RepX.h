// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2011 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  
#ifndef PX_REPXH
#define PX_REPXH
#include "common/PxPhysXCommon.h"
#include "PxSimpleTypes.h"
#include "PxSceneDesc.h"
#include "common/PxStream.h"

class PxRigidDynamicDesc;
class PxRigidStaticDesc;
class PxTriangleMesh;
class PxCooking;
class PxMaterial;
class PxPhysics;
class PxHeightField;

namespace physx { namespace shdfnd3 {
	class PxFileBuf;
} }

namespace physx {
	class PxStringTable;
}

namespace physx { namespace repx {
	using namespace physx::pubfnd;
	using namespace physx::shdfnd3;

	typedef const void* TRepXId;
	typedef void*		TDescriptor;
	class RepXMemoryAllocator;
	class RepXWriter;
	class RepXReader;
	class MemoryBuffer;

	//Contains the mapping of id to object created when a repx object
	//is added to a instantiated.
	struct RepXObject
	{
		///Identifies the extension meant to handle this object
		const char*			mTypeName;
		///Pointer to the live object this was created from
		const void*			mLiveObject;
		///Id given to this object at some point
		const void*			mId;
		RepXObject( const char* inTypeName = "", const void* inLiveObject = NULL, const void* inId = NULL )
			: mTypeName( inTypeName )
			, mLiveObject( inLiveObject )
			, mId( inId )
		{
		}
		bool isValid() const { return mLiveObject != NULL; }
	};

	template<typename TDataType>
	class RepXScopedPtr
	{
		TDataType* mObject;
		RepXScopedPtr( const RepXScopedPtr& inOther );
		RepXScopedPtr& operator=( const RepXScopedPtr& inOther );
	public:

		PX_INLINE RepXScopedPtr( TDataType* inObj )
			: mObject( inObj )
		{
		}
		PX_INLINE ~RepXScopedPtr() { if ( mObject ) mObject->destroy(); }
		PX_INLINE TDataType* release() { TDataType* retval( mObject ); mObject = NULL; return retval; }
		PX_INLINE operator TDataType* () { return mObject; }
		PX_INLINE TDataType* operator*() { return mObject; }
		PX_INLINE TDataType* operator&() { return mObject; }
		PX_INLINE TDataType* operator->() { return mObject; }
	};

	/**
	 *	Two way mapping from repx id to repx object.
	 *	Clients can implement this themselves to provide objects
	 *	to the repx system that haven't been loaded/added to a collection
	 *	yet.  Otherwise, a default implementation can be created.
	 */
	class RepXIdToRepXObjectMap
	{
	protected:
		virtual ~RepXIdToRepXObjectMap(){}
	public:
		virtual void destroy() = 0;
		virtual void addLiveObject( RepXObject inLiveObject ) = 0;
		virtual RepXObject getLiveObjectFromId( const void* inId ) = 0;
		virtual const void* getIdForLiveObject( const void* inLiveObject ) = 0;

		static RepXIdToRepXObjectMap* create(PxAllocatorCallback& inAllocator);
	};
	class RepXScopedIdToRepXObjectMap : public RepXScopedPtr<RepXIdToRepXObjectMap>
	{
	public:
		RepXScopedIdToRepXObjectMap( RepXIdToRepXObjectMap* inMap )
			: RepXScopedPtr<RepXIdToRepXObjectMap>( inMap )
		{
		}
	};

	/**
	 *	Arguments required to instantiate a repx collection.
	 *	Extra arguments can be added to the object map under
	 *	special ids.
	 */
	struct RepXInstantiationArgs
	{
		PxCooking*			mCooker;
		PxPhysics*			mPhysics;
		PxStringTable*		mStringTable;
		RepXInstantiationArgs( PxCooking* inCooking //Must have one of these
							, PxPhysics* inPhysics //Must have one of these
							, PxStringTable* inStringTable ) //String table is optional.
			: mCooker( inCooking )
			, mPhysics( inPhysics )
			, mStringTable( inStringTable )
		{
		}
	};

	/**
	 *	A repx extension provides the ability to capture a live
	 *	object to a descriptor or static state and the ability to
	 *	write that state out to a file.  Objects allocated
	 *	by the extension using the allocator are freed when the
	 *	collection itself is freed.
	 *
	 *	RepXCoreExtensions.cpp implements a set of extensions
	 *	for the core PhysX types.
	 */
	class RepXExtension
	{
	protected:
		virtual ~RepXExtension(){}
	public:

		virtual void destroy() = 0;
		/**
		 *	The type this extension is meant to operate on.  Refers to
		 *	RepXObject::mTypeName
		 */
		virtual const char* getTypeName() = 0;
		
		/**
		 *	Convert from a RepX object to a key-value pair hierarchy
		 *	
		 *	/param[in] inLiveObject The object to convert to the passed in descriptor.
		 *	/param[in] inIdMap The map to use to find ids of references of this object.
		 *	/param[in] ioWriter Interface to write data to.
		 *	/param[in] MemoryBuffer used to for temporary allocations
		 */
		virtual void objectToFile( RepXObject inLiveObject, RepXIdToRepXObjectMap* inIdMap, RepXWriter& inWriter, MemoryBuffer& inTempBuffer ) = 0;

		/**
		 *	Convert from a descriptor to a live object.  Must be an object of this extension type.
		 *
		 *	/param[in] ioReader The inverse of the writer, a key-value pair database.
		 *	/param[in] inAllocator An allocator to use for temporary allocations.  These will be freed after instantiation completes.
		 *	/param[in] inArgs The arguments used in create resources and objects.
		 *	/param[in] inIdMap The id map used to find references.
		 *
		 *	/return The new live object.  It can be an invalid object if the instantiation cannot take place.
		 */
		virtual RepXObject fileToObject( RepXReader& inReader, RepXMemoryAllocator& inAllocator, RepXInstantiationArgs& inArgs, RepXIdToRepXObjectMap* inIdMap ) = 0;
	};

	/**
	 *	The result of adding an object to the collection.
	 */
	struct RepXAddToCollectionResult
	{
		enum Enum
		{
			Success,
			ExtensionNotFound,
			InvalidParameters, //Null data passed in.
			AlreadyInCollection,
		};

		const void* mCollectionId;
		Enum		mResult;

		RepXAddToCollectionResult( Enum inResult = Success, const void* inId = NULL )
			: mCollectionId( inId )
			, mResult( inResult )
		{
		}
		bool isValid() { return mResult == Success && mCollectionId != NULL; }
	};

	/**
	 *	A result of attempting to instantiate an item in the repx collection.
	 *	The collectionId was the id the object has in the collection.
	 *	The live object contains a new id generated from the address of the scene
	 *	object so that all the objects have valid ids.
	 */
	struct RepXInstantiationResult
	{
		const void*		mCollectionId;
		void*			mLiveObject;
		const char*		mExtensionName;

		RepXInstantiationResult( const void* inCollId, void* inLiveObject, const char* inExtensionName )
			: mCollectionId( inCollId )
			, mLiveObject( inLiveObject )
			, mExtensionName( inExtensionName )
		{
		}
	};

	class RepXInstantiationResultHandler
	{
	protected:
		virtual ~RepXInstantiationResultHandler(){}
	public:
		virtual void addInstantiationResult( RepXInstantiationResult inResult ) = 0;
	};

	/**
	 *	A RepX collection contains a set of static data objects that can be transformed
	 *	into live objects.  It uses extensions to do two transformations:
	 *	live object <-> collection object (descriptor)
	 *	collection object <-> file system.
	 *
	 *	A live object is considered to be something live in the physics
	 *	world such as a material or a rigidstatic.
	 *
	 *	A collection object is a piece of data from which a live object
	 *	of identical characteristics can be created.  
	 *
	 *	References to other objects must pass through the id system.  Currently
	 *	all objects added to the repx collection change any live object ptrs
	 *	they have into repx ids using a supplied id map.  Its id is added
	 *	to the map when it itself is added to the collection.
	 *	Thus dependant objects must be added after their parent
	 *	dependencies i.e. a material must be added before a shape that
	 *	refers to that material is added; etc.  If a cycle were to occur
	 *	clients must break the cycle themselves by added the ids
	 *	to the map before any elements of the cycle are added to the collection.
	 *
	 *	Similarly, when objects are instantiated the map is used to convert back
	 *	from id to live object.  Newly instantiated objects are added to the map
	 *	under either their new auto-generated id or from the id in the file when
	 *	they were serialized.  Buffers such as materials or triangle meshes must be
	 *	added under their original ids.  Also, if the same collection is going to be
	 *	instantiated multiple times most likely users will require a new map
	 *	or will be required to clear the existing map in between instantiations.
	 *	In the current map implementation if a newly added value conflicts with an
	 *	existing value the map keeps the old value and ignores the new value.
	 *	
	 *	
	 *	Clients need to pass in object maps so that objects can resolve
	 *	references.  In addition, objects must be added in an order such that
	 *	references can be resolved in the first place.  So objects must be added
	 *	to the collection *after* objects they are dependent upon.
	 *
	 *	When deserializing from a file, the collection will allocate char*'s that will
	 *	not be freed when the collection itself is freed.  The user must be responsible
	 *	for these character allocations.
	 */
	class RepXCollection
	{
	protected:
		virtual ~RepXCollection(){}

	public:
		virtual void destroy() = 0;

		/**
		 *	Get the scale that was set at collection creation time or at load time.
		 *	If this is a loaded file and the source data does not contain a scale
		 *	this value will be invalid (PxTolerancesScale::isValid()).
		 */
		virtual PxTolerancesScale getTolerancesScale() const = 0;

		/**
		 *	Set the up vector on this collection.  The up vector is saved with the collection.
		 *
		 *	If the up vector wasn't set, it will be (0,0,0).
		 */
		virtual void  setUpVector( const PxVec3& inUpVector ) = 0;

		/**
		 * If the up vector wasn't set, it will be (0,0,0).  Else this will be the up vector
		 * optionally set when the collection was created.
		 */
		virtual PxVec3	getUpVector() const = 0;

		/**
		 *	Add an object to the collection.  The live object map is used by extensions to 
		 *	created ids for objects this object refers to.
		 *
		 *	/param[in] inObject Object to save to descriptor format and add to collection.
		 *	/param[in] inLiveObjectMap Map used to find references.  Also, the live object is added to this map upon successful completion.
		 *	
		 *	/return The result of the add operations.
		 */
		virtual RepXAddToCollectionResult addRepXObjectToCollection( const RepXObject& inObject, RepXIdToRepXObjectMap* inLiveObjectIdMap ) = 0;
		/**
		 *	Instantiate this collection.  Each instantiated object creates a new scene object mapped to a new id.
		 *	The list of the old-id-to-new-scene-objects is returned.
		 *	The id map is used twice; to resolve references and when an object has been instantiated.
		 *	
		 *	Instantiated objects can be added to the id map either under their new id or under the id
		 *	their descriptors were under originally.  A collection of buffers should be added under their
		 *	original ids.  A collection of objects referencing those buffers that will be instanced
		 *	several times should be added under new ids.
		 *
		 *	/param[in] inArgs Data arguments to the instantiation function.
		 *	/param[in] inLiveObjectIdMap Map used for references.  Results of the instantiation are added to this map.
		 *	/param[in] inResultHandler container for the new results along with their instantiation ids.  May be NULL.
		 *	/param[in] inAddOriginalIdsToObjectMap When true, add new objects to map under original ids.  Else add under new ids.
		 */
		virtual void instantiateCollection( RepXInstantiationArgs inArgs, RepXIdToRepXObjectMap* inLiveObjectIdMap
											, RepXInstantiationResultHandler* inResultHandler, bool inAddOriginalIdsToObjectMap = true ) = 0;
		/**
		 *	Save this collection out to a file stream.  Uses the extensions to perform 
		 *	collection object->file conversions.
		 *
		 *	/param[in] inFilestream Write-only stream to save collection out to.
		 */
		virtual void save( PxStream* inFilestream ) = 0;

		/** 
		 *	Create a new empty collection referencing these extensions.  The extensions will be destroyed
		 *	when the collection itself is destroyed.
		 *	
		 *	\param[in] inExtensions Extensions used to provide the collection with add/remove and serialization capabilities.
		 *	\param[in] Scale - scale that this collection is created with.  This is saved with the collection so future
		 *		users of the data can know if the current PxPhysics::getScale matches.
		 *	\param[in] inAllocator Allocator used for building the collection.
		 *	
		 *	
		 *	/return Empty collection.
		 */
		static RepXCollection* create( RepXExtension** inExtensions, PxU32 inNumExtensions, const PxTolerancesScale& inScale, PxAllocatorCallback& inAllocator );
		/**
		 *	Create a collection from a given file using these extensions.  The extensions will be destroyed
		 *	when the collection itself is destroyed.  
		 *
		 *	!!Char* name properties are not released (PxActor->getName(), PxShape->getName()) when the collection
		 *  itself is released.  Thus these pointers become floating pointers.  If you want to manage them
		 *	you can track outstanding allocations that are unreleased and release them when you know you don't
		 *	need them!!
		 *	
		 *	\param[in] inFilename The name of the resource to load to create this extension.
		 *	\param[in] inExtensions Array of extensions used to provide the collection with add/remove and serialization capabilities.
		 *	\param[in] inAllocator Allocator used for collection allocations and const char* name allocations.
		 *	
		 *	\return new collection with items in the file transformed into a descriptor state.
		 */
		static RepXCollection* create( const char* inFilename, RepXExtension** inExtensions, PxU32 inNumExtensions, PxAllocatorCallback& inAllocator );
		
		/**
		 *	Like the above item but takes a PxFileBuf and thus isn't dependent upon an actual file
		 *	on the filesystem.
		 *
		 *	!!Char* name properties are not released (PxActor->getName(), PxShape->getName()) when the collection
		 *  itself is released.  Thus these pointers become floating pointers.  If you want to manage them
		 *	you can track outstanding allocations that are unreleased and release them when you know you don't
		 *	need them!!
		 */
		static RepXCollection* create( PxFileBuf* inCollectionData, RepXExtension** inExtensions, PxU32 inNumExtensions, PxAllocatorCallback& inAllocator );
		/** 
		 *	Save this collection to a filename.  This is a helper function; all it does is
		 *	convert inFilename -> PxStream then call inCollection->save()
		 *	
		 *	/param inCollection Collection to save to file.
		 *	/param inFilename Name of file to save data to.
		 */
		static void save( RepXCollection* inCollection, const char* inFilename );
	};

	class RepXScopedCollection : public RepXScopedPtr<RepXCollection>
	{
	public:

		RepXScopedCollection( RepXCollection* inCollection )
			: RepXScopedPtr<RepXCollection>( inCollection )
		{
		}
	};
} }

	 
#endif //PX_REPXH