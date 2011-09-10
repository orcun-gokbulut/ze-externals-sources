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


#ifndef PX_PHYSICS_NX_PHYSICS
#define PX_PHYSICS_NX_PHYSICS


/** \addtogroup physics
@{
*/

#include "PxPhysX.h"
#include "common/PxStream.h"
#include "common/PxSerialFramework.h"
#include "PxTransform.h"

namespace physx
{
	class PxProfileZone;
	class PxProfileZoneManager;
}

namespace PVD
{
	class PvdConnectionManager;
}

class PxGeometry;

struct PxCookingValue
{
	enum Enum
	{
		/**
		Version numbers follow this format:

		Version = 16bit|16bit

		The high part is increased each time the format changes so much that
		pre-cooked files become incompatible with the system (and hence must
		be re-cooked)

		The low part is increased each time the format changes but the code
		can still read old files. You don't need to re-cook the data in that
		case, unless you want to make sure cooked files are optimal.
		*/
		eCONVEX_VERSION_PC,
		eMESH_VERSION_PC,
		eCONVEX_VERSION_XENON,
		eMESH_VERSION_XENON,
		eCONVEX_VERSION_PLAYSTATION3,
		eMESH_VERSION_PLAYSTATION3,
	};
};

/**
\brief Reads an internal value (cooking format version).

\param[in] cookValue See #PxCookingValue
*/
PX_C_EXPORT PX_PHYSX_CORE_API PxU32 PX_CALL_CONV PxGetValue(PxCookingValue::Enum cookValue);

/**
\brief Abstract singleton factory class used for instancing objects in the Physics SDK.

In addition you can use PxPhysics to set global parameters which will effect all scenes,
create triangle meshes.

You can get an instance of this class by calling PxCreatePhysics().

@see PxCreatePhysics() PxScene PxVisualizationParameter PxTriangleMesh PxConvexMesh
*/
class PxPhysics
{
public:

// PX_SERIALIZATION
	virtual void getMetaData(PxSerialStream& stream)	const = 0;

	virtual bool registerClass(PxType type, PxClassCreationCallback callback) = 0;

	/**
	\brief Creates a user references object.

	User references are needed when a collection contains external references, either to
	another collection (when serializing subsets) or to user objects.

	@see releaseUserReferences() PxUserReferences
	*/
	virtual	PxUserReferences*	createUserReferences() = 0;

	/**
	\brief Deletes a user references object.

	@see createUserReferences() PxUserReferences
	*/
	virtual	void				releaseUserReferences(PxUserReferences* ref) = 0;

	/**
	\brief Creates a collection object.

	Objects can only be serialized or deserialized through a collection.
	For serialization, users must add objects to the collection and serialize the collection as a whole.
	For deserialization, the system gives back a collection of deserialized objects to users.

	\return The new collection object.

	@see releaseCollection() PxCollection
	*/
	virtual	PxCollection*		createCollection() = 0;

	/**
	\brief Deletes a collection object.

	This function only deletes the collection object, i.e. the container class. It doesn't delete objects
	that are part of the collection.

	@see createCollection() PxCollection
	*/
	virtual	void				releaseCollection(PxCollection*) = 0;

	/**
	\brief Adds collected objects to a scene.

	This function adds all objects contained in the input collection to the input scene. This is
	typically used after deserializing the collection, to populate the scene with deserialized
	objects.

	\param[in] collection Objects to add to the scene. See #PxCollection
	\param[in] scene Scene to which collected objects will be added. See #PxScene

	@see PxCollection PxScene
	*/
	virtual	void				addCollection(const PxCollection& collection, PxScene* scene) = 0;

	/**
	\brief Removes collected objects from a scene.

	This is still experimental and should not be used yet.
	*/
	virtual	void				releaseCollected(PxCollection&, PxScene*) = 0;
//~PX_SERIALIZATION

	/**
	\brief Destroys the instance it is called on.

	Use this release method to destroy an instance of this class. Be sure
	to not keep a reference to this object after calling release.
	Avoid release calls while a scene is simulating (in between simulate() and fetchResults() calls).

	Note that this must be called once for each prior call to PxCreatePhysics, as
	there is a reference counter. Also note that you mustn't destroy the allocator or the error callback (if available) until after the
	reference count reaches 0 and the SDK is actually removed.

	Releasing an SDK will also release any scenes, triangle meshes, convex meshes, heightfields, and deformable
	meshes created through it, provided the user hasn't already done so.

	@see PxCreatePhysics()
	*/
	virtual	void release() = 0;

	/**
	\brief Creates a scene.

	The scene can then create its contained entities.

	\param[in] sceneDesc Scene descriptor. See #PxSceneDesc
	\return The new scene object.

	<b>Limitations:</b>

	The number of scenes that can be created is limited by the amount of memory available, and since this 
	amount varies dynamically as memory is allocated and deallocated by the PhysX SDK and by other software 
	components there is in general no way to statically determine the maximum number of scenes which can be 
	created at a given point in the simulation.

	However, scenes are built from lower-level objects called contexts. There is a limit of 64 contexts; a scene will 
	take 1 context. This places an absolute limit on the maximum number of scenes regardless of the memory available.

	@see PxScene PxScene.release() PxSceneDesc
	*/
	virtual PxScene*			createScene(const PxSceneDesc& sceneDesc) = 0;

	/**
	\brief Gets number of created scenes.

	\return The number of scenes created.

	@see getScene()
	*/
	virtual PxU32				getNbScenes()			const	= 0;

	/**
	\brief Writes the array of scene pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the scene pointers in the array is not specified.

	\param[out] userBuffer The buffer to receive scene pointers.
	\param[in] bufferSize The number of scene pointers which can be stored in the buffer.
	\return The number of scene pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbScenes() PxScene
	*/
	virtual	PxU32				getScenes(PxScene** userBuffer, PxU32 bufferSize) const = 0;



	/**
	\brief Creates a static rigid actor with the specified pose and all other fields initialized
	to their default values.
	
	\param[in] pose the initial pose of the actor. Must be a valid transform

	@see PxRigidStatic
	*/

	virtual PxRigidStatic*      createRigidStatic(const PxTransform& pose) = 0;



	/**
	\brief Creates a dynamic rigid actor with the specified pose and all other fields initialized
	to their default values.
	
	\param[in] pose the initial pose of the actor. Must be a valid transform

	@see PxRigidDynamic
	*/

	virtual PxRigidDynamic*      createRigidDynamic(const PxTransform& pose) = 0;


	/**
	\brief Creates a constraint shader.

	\note A constraint shader will get added automatically to the scene the two linked actors belong to
	
	\param[in] constraintDesc The descriptor for the shader to create. 
	\return The new shader.

	@see PxConstraint PxConstraintDesc
	*/
	virtual PxConstraint*      createConstraint(const PxConstraintDesc& constraintDesc)		= 0;

	/**
	\brief Creates an articulation with all fields initialized to their default values.
	
	\return the new articulation

	@see PxRigidDynamic
	*/

	virtual PxArticulation*      createArticulation() = 0;



// PX_AGGREGATE
	/**
	\brief Creates an aggregate with the specified maximum size and selfCollision property.
	
	\param[in] maxSize the maximum number of actors that may be placed in the aggregate
	\param[in] enableSelfCollision whether the aggregate supports self-collision
	\return The new aggregate.

	@see PxAggregate PxAggregateDesc
	*/
	virtual	PxAggregate*		createAggregate(PxU32 maxSize, bool enableSelfCollision)	= 0;
//~PX_AGGREGATE



#if PX_USE_PARTICLE_SYSTEM_API
	/**
	\brief Creates a particle system. 
	
	PxParticleSystemDesc::isValid() must return true.

	\param[in] particleSystemDesc Description of the particle system object to create. See #PxParticleSystemDesc.
	\return The new particle system.
	*/
	virtual PxParticleSystem*	createParticleSystem(const PxParticleSystemDesc& particleSystemDesc) = 0;

	/**
	\brief Creates a particle fluid. 
	
	PxParticleFluidDesc::isValid() must return true.

	\param[in] particleFluidDesc Description of the particle system object to create. See #PxParticleFluidDesc.
	\return The new particle fluid.
	*/
	virtual PxParticleFluid*	createParticleFluid(const PxParticleFluidDesc& particleFluidDesc) = 0;
#endif

#if PX_USE_DEFORMABLE_API
	/**
	Creates a deformable. PxDeformableDesc::isValid() must return true.

	\param deformableDesc Description of the deformable object to create. See #PxDeformableDesc.
	\return The new deformable.

	@see PxDeformableDesc PxDeformable
	*/
	virtual PxDeformable*		createDeformable(const PxDeformableDesc& deformableDesc)		= 0;

	/**
	Creates a deformable attachment.

	\param deformable The deformable to attach.
	\param shape The shape to attach to ('0' for static attachments).
	\param nbVertices The number of vertices to attach.
	\param vertexIndices The indices of the vertices to attach.
	\param positions The attachment positions in shape-local coordinates or world space for world attachments.
	\param flags Flags for each attached vertex: One or two way interaction, tearable or non-tearable.

	\return The new attachment.

	@see PxDeformableDesc PxDeformable
	*/
	virtual PxAttachment*       createAttachment(PxDeformable& deformable , PxShape* shape, PxU32 nbVertices, const PxU32* vertexIndices, const PxVec3* positions, const PxU32* flags) = 0;
#endif

	/**
	\brief Creates a new material with default properties.

	\return The new material.

	\param staticFriction the coefficient of static friction
	\param dynamicFriction the coefficient of dynamic friction
	\param restitution the coefficient of restitution

	@see PxMaterial 
	*/
	virtual PxMaterial*        createMaterial(PxReal staticFriction, PxReal dynamicFriction, PxReal restitution)		= 0;


	/**
	\brief Return the number of materials that currently exist.

	\return Number of materials.

	@see getMaterials()
	*/
	virtual PxU32				getNbMaterials() const = 0;

	/**
	\brief Writes the array of material pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the materials in the array is not specified.

	\param[out] userBuffer The buffer to receive material pointers.
	\param[in] bufferSize The number of material pointers which can be stored in the buffer.
	\return The number of material pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbMaterials() PxMaterial
	*/
	virtual	PxU32				getMaterials(PxMaterial** userBuffer, PxU32 bufferSize) const = 0;

	/**
	\brief Creates a triangle mesh object.

	This can then be instanced into #PxShape objects.

	\param[in] stream The triangle mesh stream.
	\return The new triangle mesh.

	@see PxTriangleMesh PxTriangleMesh.release() PxStream
	*/
	virtual PxTriangleMesh*    createTriangleMesh(const PxStream& stream)					= 0;

	/**
	\brief Return the number of triangle meshes that currently exist.

	\return Number of triangle meshes.

	@see getTriangleMeshes()
	*/
	virtual PxU32				getNbTriangleMeshes() const = 0;

	/**
	\brief Writes the array of triangle mesh pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the triangle meshes in the array is not specified.

	\param[out] userBuffer The buffer to receive triangle mesh pointers.
	\param[in] bufferSize The number of triangle mesh pointers which can be stored in the buffer.
	\return The number of triangle mesh pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbTriangleMeshes() PxTriangleMesh
	*/
	virtual	PxU32				getTriangleMeshes(PxTriangleMesh** userBuffer, PxU32 bufferSize) const = 0;

	/**
	\brief Creates a PxHeightField object.

	This can then be instanced into #PxShape objects.

	\param[in] heightFieldDesc The descriptor to load the object from.
	\return The new height field object.

	@see PxHeightField PxHeightField.release() PxHeightFieldDesc PxHeightFieldGeometry PxShape
	*/
	virtual PxHeightField*		createHeightField(const PxHeightFieldDesc& heightFieldDesc) = 0;

	/**
	\brief Return the number of heightfields that currently exist.

	\return Number of heightfields.

	@see getHeightFields()
	*/
	virtual PxU32				getNbHeightFields() const = 0;

	/**
	\brief Writes the array of heightfield pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the heightfields in the array is not specified.

	\param[out] userBuffer The buffer to receive heightfield pointers.
	\param[in] bufferSize The number of heightfield pointers which can be stored in the buffer.
	\return The number of heightfield pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbHeightFields() PxHeightField
	*/
	virtual	PxU32				getHeightFields(PxHeightField** userBuffer, PxU32 bufferSize) const = 0;

	/**
	\brief Creates a convex mesh object.

	This can then be instanced into #PxShape objects.

	\param[in] mesh The stream to load the convex mesh from.
	\return The new convex mesh.

	@see PxConvexMesh PxConvexMesh.release() PxStream createTriangleMesh() PxConvexMeshGeometry PxShape
	*/
	virtual PxConvexMesh*      createConvexMesh(const PxStream& mesh)					= 0;

	/**
	\brief Return the number of convex meshes that currently exist.

	\return Number of convex meshes.

	@see getConvexMeshes()
	*/
	virtual PxU32				getNbConvexMeshes() const = 0;

	/**
	\brief Writes the array of convex mesh pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the convex meshes in the array is not specified.

	\param[out] userBuffer The buffer to receive convex mesh pointers.
	\param[in] bufferSize The number of convex mesh pointers which can be stored in the buffer.
	\return The number of convex mesh pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbConvexMeshes() PxConvexMesh
	*/
	virtual	PxU32				getConvexMeshes(PxConvexMesh** userBuffer, PxU32 bufferSize) const = 0;

#if PX_USE_DEFORMABLE_API
	/**
	\brief Creates a deformable mesh from a cooked deformable mesh stored in a stream.

	Stream has to be created with PxCookDeformableMesh(). 

	\return The new deformable mesh.
	*/
	virtual PxDeformableMesh*	createDeformableMesh(const PxStream& stream) = 0;

	/**
	\brief Return the number of deformable meshes that currently exist.

	\return Number of deformable meshes.

	@see getDeformableMeshes()
	*/
	virtual PxU32				getNbDeformableMeshes() const = 0;

	/**
	\brief Writes the array of deformable mesh pointers to a user buffer.
	
	Returns the number of pointers written.

	The ordering of the deformable meshes in the array is not specified.

	\param[out] userBuffer The buffer to receive deformable mesh pointers.
	\param[in] bufferSize The number of deformable mesh pointers which can be stored in the buffer.
	\return The number of deformable mesh pointers written to userBuffer, this should be less or equal to bufferSize.

	@see getNbDeformableMeshes() PxDeformableMesh
	*/
	virtual	PxU32				getDeformableMeshes(PxDeformableMesh** userBuffer, PxU32 bufferSize) const = 0;
#endif

	/**
	\brief Returns the simulation tolerance parameters.  
	\return The current simulation tolerance parameters.  
	*/
	virtual const PxTolerancesScale&		getTolerancesScale() const = 0;

	/**
	\brief Retrieves the Foundation instance.
	\return A reference to the Foundation object.
	*/
	virtual PxFoundation&		getFoundation() = 0;

	/**
	\brief Retrieves the PhysX Visual Debugger.
	\return A pointer to the PxVisualDebugger. Can be NULL if PVD is not supported on this platform.
	*/
	virtual PxVisualDebugger*	getVisualDebugger()	= 0;

	/**
		The factory manager allows notifications when a new
		connection to pvd is made.  It also allows the users to specify
		a scheme to handle the read-side of a network connection.  By default, 
		the SDK specifies that a thread gets launched which blocks reading
		on the network socket.
	
		\return A valid manager *if* the SDK was compiled with PVD support.  Null otherwise.
	*/
	virtual PVD::PvdConnectionManager* getPvdConnectionManager() = 0;

	/*
	\brief Retrieves the profile sdk manager.
	*	The profile sdk manager manages collections of SDKs and objects that are interested in 
	*	receiving events from them.  This is the hook if you want to write the profiling events
	*	from multiple SDK's out to a file.
	\return The SDK's profiling system manager.
	*/
	virtual physx::PxProfileZoneManager& getProfileZoneManager() = 0;

#if PX_SUPPORT_GPU_PHYSX
	/**
	\brief returns PxPhysicsGpu to configure execution on the GPU. 
	
	@see PxPhysicsGpu PxParticleBaseFlag::eGPU 
	*/
	virtual class PxPhysicsGpu&	getPhysicsGpu() = 0;
#endif

	PX_DEPRECATED virtual PxRigidStatic*    createRigidStatic(const class PxRigidStaticDesc& staticDesc) = 0;
	PX_DEPRECATED virtual PxRigidDynamic*   createRigidDynamic(const class PxRigidDynamicDesc& dynamicDesc) = 0;
	PX_DEPRECATED virtual PxArticulation*	createArticulation(const class PxArticulationDesc& articulationDesc) = 0;
	PX_DEPRECATED virtual PxMaterial*       createMaterial(const class PxMaterialDesc& materialDesc)			= 0;

};


/**
\brief Creates an instance of the physics SDK.

Creates an instance of this class. May not be a class member to avoid name mangling.
Pass the constant PX_PHYSICS_VERSION as the argument.
Because the class is a singleton class, multiple calls return the same object. However, each call must be
matched by a corresponding call to PxPhysics::release(), as the SDK uses a reference counter to the singleton.

NOTE: Calls after the first will not change the allocator used in the SDK, but they will affect the output stream.

\param version Version number we are expecting(should be PX_PHYSICS_VERSION)
\param allocator User supplied interface for allocating memory(see #PxAllocatorCallback)
\param errorCallback User supplied interface for reporting errors and displaying messages(see #PxErrorCallback)
\param scale values used to determine default tolerances for objects at creation time
\param trackOustandingAllocations true if you want to track memory allocations 
			so a debugger connection partway through your physics simulation will get
			an accurate map of everything that has been allocated so far.  This could have a memory
			and performance impact on your simulation hence it defaults to off.
*/
PX_C_EXPORT PX_PHYSX_CORE_API PxPhysics* PX_CALL_CONV PxCreatePhysics(PxU32 version
																		, PxAllocatorCallback& allocator
																		, PxErrorCallback& errorCallback
																		, const PxTolerancesScale& scale
																		, bool trackOustandingAllocations = false );


/**
\brief Retrieves the Physics SDK after it has been created.

Before using this function the user must call #PxCreatePhysics(). If #PxCreatePhysics()
has not been called then NULL will be returned.
*/
PX_C_EXPORT PX_PHYSX_CORE_API PxPhysics* PX_CALL_CONV PxGetPhysics();


/**
\brief Retrieves the Foundation SDK after it has been created.
*/
PX_C_EXPORT PX_PHYSX_CORE_API PxFoundation* PxGetFoundation();

/** @} */
#endif
