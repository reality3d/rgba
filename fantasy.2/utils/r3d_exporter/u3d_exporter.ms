(--Bloque del script

--Cabecera

geometrycont=#() --Container que guarda sólo la geometría(geometry también guarda los bones)
bonecont=#() --Container que guarda los bones
cameralist=#()
totalcontrollers=#()
materialids=#()


sliderTime=animationRange.start
undo off(
--Save File dialog
--Save binary read only
filename=getSaveFileName caption:"Save .u3d"  types:"ultimate3d(*.u3d)|*.u3d"
if(filename!=undefined) then -- Abrimos el fichero
(
	file=fopen filename "wb" 
	 
	-----------------------------------------------	
	-- 			FUNCIONES BASE
	-----------------------------------------------	
	fn lightclass i=
	(
		case (Classof i) of
		(
			Omnilight:
				(
					return 0
				)	
			TargetDirectionallight:
				(
					return 1

				)	
			targetSpot:			
				(
					return 2
				)
			default:
				(
					return -1
				)
			
		)
	)
	
	fn writestring_withoutnull output i= 
	(
		for j=1 to i.count do
			WriteByte output (bit.charasint i[j]) #unsigned		
	)	

	fn write_transform_matrix i type= 
	(
		objpos=i.controller.position
		
		--Position	
		WriteFloat file objpos.x
		WriteFloat file objpos.z
		WriteFloat file objpos.y	

		if(type=="CAMS" or type=="CTGT" or type=="LITE") then
		(
			--Rotation Quat
			objrot=i.transform.rotation--(i.rotation-transform_matrix.rotation)
			WriteFloat file objrot.x
			WriteFloat file objrot.z
			WriteFloat file objrot.y			
			WriteFloat file objrot.w
		)
		else
		(
			--Rotation Quat
			objrot=inverse(i.controller.rotation)--(i.rotation-transform_matrix.rotation)
			WriteFloat file objrot.x
			WriteFloat file objrot.z
			WriteFloat file objrot.y			
			WriteFloat file (-objrot.w)
		)
		--Scale
		objscale=i.controller.scale
		WriteFloat file objscale.x
		WriteFloat file objscale.z
		WriteFloat file objscale.y			
			
		
		objpivot=i.pivot
		--Pivot. The same as obj	
		WriteFloat file objpivot.x
		WriteFloat file objpivot.z
		WriteFloat file objpivot.y	
			
		/*	
		--Pivot
		WriteFloat file i.pivot.x
		WriteFloat file i.pivot.z
		WriteFloat file i.pivot.y	
		*/

	)		

	fn tcb_controllers nodeid=
	(
		nodeid.pos.controller = tcb_position ()
		if(classOf nodeid!=targetCamera and classOf nodeid!=targetSpot and classOf nodeid!=TargetDirectionallight) then
		(
			nodeid.rotation.controller = tcb_rotation ()
		)
		nodeid.scale.controller = tcb_scale ()
		if(classOf nodeid==Targetcamera or classOf nodeid==targetSpot or classOf nodeid==TargetDirectionallight) then
		(
			nodeid.controller.roll_angle.controller=tcb_float()
			if(classOf nodeid==targetCamera) then
			(
				if(nodeid.fov.controller!=undefined) then
				(
					nodeid.fov.controller= tcb_float()
				)
			)
		)
	)	
	--Headers of chunks(camera,mesh,etc..)
	fn chunkheader i type= 
	(

		WriteString file i.name			
		WriteLong file i.inode.handle #signed --Object ID
	
		if(i.parent==undefined) then
		(
			WriteLong file 0 #signed
		)
		else
		(			
			WriteLong file i.parent.inode.handle #signed
		)
		if(type=="CAMS") then
		(
			WriteLong file i.target.inode.handle #signed --Target id
		)
		if(type=="LITE") then
		(
			target=-1
			if( (lightclass i)>0) then 
			(
				target=i.target.inode.handle
			)
			WriteLong file target #signed --Target id

		)
		--if(i.isHidden) then
		--{
			WriteByte file 0x00 #unsigned
		--}
		--else
		--{
		--	WriteByte file 0x01 #unsigned
		--}
		write_transform_matrix i type --Write Object Transform Matrix

	)	
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	

	writestring_withoutnull file "UTO!" 

	
	WriteByte file 0 #unsigned --format version major
	WriteByte file 9 #unsigned --format version minor
	
	writestring_withoutnull file "MAIN"
	WriteLong file animationRange.start #signed
	WriteLong file animationRange.end #signed
	WriteLong file frameRate #signed
	WriteLong file ticksPerFrame #signed
	
	WriteFloat file 1.0
	
	geometry_helpers=join (geometry as array) (helpers as array)
	for i in geometry_helpers do
	(
		if(not i.boneEnable) then
		(
			append geometrycont i
		)
		else
		(
			append bonecont i
		)
		
	)
	reallights=#() -- light container has targetlights too
	for i in lights do
	(
		if (Classof i != TargetObject) then
		(
			append reallights i
		)
	)
	totalobjects=join (join (geometry as array) (cameras as array)) (lights as array)
	WriteLong file geometrycont.count #signed
	WriteLong file (cameras.count/2) #signed  --Targets included
	WriteLong file reallights.count #signed
	WriteLong file sceneMaterials.count #signed
	for i in cameras do
	(
		if(classOf i==targetCamera) then
		(
			append cameralist i
		)
		
	)
	--Controllers of the scene
	for i in totalobjects do
	(
		tcb_controllers i --Convert to tcb
		if(i.position.controller.keys.count>0) then
		(
			append totalcontrollers #(i.position.controller,"position",i.inode.handle)
		)
		if(classOf i!=targetCamera and classOf i!=targetSpot and classOf i!=TargetDirectionallight) then 
		(
			if(i.rotation.controller.keys.count>0) then
			(
				append totalcontrollers #(i.rotation.controller,"rotation",i.inode.handle)
			)
		)
		if((classOf i!=targetobject) and (classOf i!=targetCamera)) then --Camera target scale not suported in fantasy
		(
			if(i.scale.controller.keys.count>0) then
			(
				append totalcontrollers #(i.scale.controller,"scale",i.inode.handle)
			)		
		)
		if(classOf i==targetCamera) then
		(
			if(i.controller.roll_angle.keys.count>0) then
			(
				append totalcontrollers #(i.controller.roll_angle.controller,"roll",i.inode.handle)
			)
			if(classOf i==targetCamera) then
			(
				if(i.fov.controller!=undefined) then
				(
					if(i.fov.controller.keys.count>0) then
					(
						append totalcontrollers #(i.fov.controller,"fov",i.inode.handle)
					)
				)
			)
		)
				
	)
	WriteLong file totalcontrollers.count #signed
	WriteLong file bonecont.count #signed
	
	-----------------------------------------------	
	-- 			MESH 
	-----------------------------------------------		
	--Con progressbar no funcionan las funciones a  skinning :|
	--progressStart "Exportando Geometria:" 
	
	writestring_withoutnull file "MESH"
	--Recorro Todos los objetos y los voy construyendo
	geometrycount=0

	for i in geometrycont do
	(
		progressUpdate (100*geometrycount/geometrycont.count)	
		
		--Turn To Mesh For Faster Export
		addModifier i (Turn_to_Mesh ())
		chunkheader i "MESH"
		--Flags
		--bit 1: has bounding sphere
		--bit 2: dynamic object (will be modified externally)
		--bit 3: skinned object
		--bit 4: cast shadows
		flags=0			
		skinmod=undefined
		for modi in i.modifiers do
		(
			if (classOf modi==skin) then
			(
				if(modi.enabled) then
				(
					flags=bit.set flags 3 true
				)
				continue
			)
		)
		themesh=i.mesh
		
		WriteLong file flags #signed
		
		WriteLong file themesh.numverts #signed
		WriteLong file themesh.numfaces #signed
		WriteLong file themesh.numtverts #signed
		WriteLong file themesh.numcpvverts #signed

		theverts=themesh.verts
		if(bit.and flags 0x04 ==0x04) then --Skinned object
		(
			WriteByte file 0x01 #unsigned  --Weighted skin
			skinmod=i.modifiers[#Skin]
			max modify mode
			modPanel.setCurrentObject skinmod ui:true
			numverts=skinOps.GetNumberVertices skinmod
			numbonespervertex=#()
			for j=1 to numverts do
			(
				tempvertexcount=skinOps.GetVertexWeightCount skinmod j
				WriteLong file tempvertexcount #signed				
				append  numbonespervertex tempvertexcount
			)
			for j=1 to numverts do
			(
				for k=1 to numbonespervertex[j] do
				(
					boneid=skinOps.GetVertexWeightBoneID skinmod j k
					--Transform the skin boneid to scene id handle
					bonename=skinOps.GetBoneName skinmod boneid 1
					bonehandle=(execute ("$" + bonename)).inode.handle
					WriteLong file bonehandle #signed
				)				
			)
			for j=1 to numverts do
			(
				for k=1 to numbonespervertex[j] do
				(
					WriteFloat file (skinOps.GetVertexWeight skinmod j k)
				)				
			)			
			theverts=i.verts
		)			
		for j in theverts do --vertex coordinates
		(
			/*if(i.parent!=undefined) then
			(
				j.position+=i.objectoffsetpos
			)*/
			offset=i.objectoffsetpos
			--format "[%,%,%]\n" j.position.x j.position.z j.position.y
			WriteFloat file (j.position.x + offset.x)
			WriteFloat file (j.position.z + offset.z)
			WriteFloat file (j.position.y + offset.y)
		)
		for j=1 to themesh.numtverts do --mapping coordinates
		(
			uv=getTVert themesh j
			WriteFloat file uv.x
			WriteFloat file uv.y
		)
		for j=1 to themesh.numcpvverts do  --vertex colors
		(
			color=getVertColor themesh j
			color=color/255   --Normalize to 0-1 because fantasys uses that range unlike 3dsmax
			WriteFloat file color.r
			WriteFloat file color.g
			WriteFloat file color.b
		)
		for j=1 to themesh.numfaces do --vertex face assignments
		(
			faceindices=getFace themesh j
			WriteLong file ((faceindices[1]-1) as integer) #signed  -- -1 because C is 0 indexed
			WriteLong file ((faceindices[2]-1) as integer) #signed
			WriteLong file ((faceindices[3]-1) as integer) #signed		
		)					
		if(themesh.numtverts>0) then
		(
			for j=1 to themesh.numfaces do --texture coord-vertex assignments
			(
				faceuvindices=getTVFace themesh j
				WriteLong file ((faceuvindices[1]-1) as integer) #signed  -- -1 because C is 0 indexed
				WriteLong file ((faceuvindices[2]-1) as integer) #signed
				WriteLong file ((faceuvindices[3]-1) as integer) #signed		
			)
		)
		
		if(themesh.numcpvverts>0) then --vertex color-face assignments
		(			
			for j=1 to themesh.numfaces do
			(
				vcindices=getVCFace themesh j				
				WriteLong file ((vcindices[1]-1) as integer) #signed
				WriteLong file ((vcindices[2]-1) as integer) #signed
				WriteLong file ((vcindices[3]-1) as integer) #signed
			)
		)

		for j=1 to themesh.numfaces do
		(
			WriteByte file 0x07 #unsigned  --All the edges visible			
		)
		if(i.material==undefined) then
		(
			titlebox="No material found on object " + i.name
			messageBox "Objects must have Materials asigned!" title:titlebox
			fclose file
		)
		else
		(
			themesh_material=(findItem materialids i.material)
			if(themesh_material==0) then
			(
				append materialids i.material
				themesh_material=materialids.count-1 -- -1 because C is 0 indexed
				--format "Material:%  ID:% \n" i.material themesh_material
			)
			else
			(
				themesh_material-=1 -- -1 because C is 0 indexed
			)
		)
		for j=1 to themesh.numfaces do
		(
			WriteLong file themesh_material #signed --Material Id
		)
		geometrycount+=1

	)
	--progressEnd()

	-----------------------------------------------	
	-- 			CAMERAS
	-----------------------------------------------	

	writestring_withoutnull file "CAMS"
	
	progressStart "Exportando Camaras:"
	
	camerascount=0
	for i in cameralist do
	(
		progressUpdate (100*camerascount/cameras.count)	

		chunkheader i "CAMS"

				
		WriteFloat file i.fov
		WriteFloat file i.nearclip
		WriteFloat file i.farclip
		
		camerascount+=1
	)
	
	progressEnd()

	-----------------------------------------------	
	-- 			CAMERA TARGETS
	-----------------------------------------------	
	writestring_withoutnull file "CTGT"
	
	progressStart "Exportando Targets de las Camaras:"
	
	camerastargetcount=0
	cameratargetlist=#()
	for i in cameras do
	(
		if(classOf i==Targetobject) then
		(
			append cameratargetlist i
		)
		
	)
	for i in cameratargetlist do
	(
		progressUpdate (100*camerastargetcount/cameras.count)	

		chunkheader i "CTGT"
		
		camerastargetcount+=1
	)
	
	progressEnd()	

	-----------------------------------------------	
	-- 			LUCES
	-----------------------------------------------	
	writestring_withoutnull file "LITE"
	progressStart "Exportando Luces:"
	
	lightcount=0
	for i in reallights do
	(
		progressUpdate (100*lightcount/lights.count)	
		
		chunkheader i "LITE"
		
		type = lightclass i
		WriteLong file type #unsigned --Light Type 0:Omni 1:Directional 2:Spot
		o_light_color=i.color
		WriteFloat file (o_light_color.r/255)
		WriteFloat file (o_light_color.g/255)
		WriteFloat file (o_light_color.b/255)
		
		WriteFloat file i.farAttenStart 
		WriteFloat file i.farAttenEnd
		if(i.useFarAtten) then
		(
			WriteByte file 1 #unsigned --Use atenuation
		)
		else
		(
			WriteByte file 0 #unsigned --No atenuation
		)
		WriteFloat file i.multiplier
		
		if(type == 1 or type == 2) then
		(
			position = i.target.position
			
			WriteFloat file position.x 
			WriteFloat file position.z 
			WriteFloat file position.y 
			if(type == 2) then
			(
			WriteFloat file (i.falloff/2.0)  --Cut-Off		
			WriteFloat file (i.hotspot as integer)  --Exponent
			)
		)
		lightcount+=1
	)
	
	progressEnd()

	-----------------------------------------------	
	-- 			MATERIAL IDS
	-----------------------------------------------	
	writestring_withoutnull file "MATS"
	
	progressStart "Exportando Materiales:"
	
	materialscount=0 -- 0 because C is 0 indexed
	for i in materialids do
	(
		progressUpdate (100*materialscount/materialids.count)	
		
		WriteString file i.name --Material name
		WriteLong file materialscount #signed
		--format "Name:% Id:%\n" i.name materialscount
		materialscount+=1
	)
	
	progressEnd()		

	-----------------------------------------------	
	-- 			BONES
	-----------------------------------------------	
	writestring_withoutnull file "BONE"

	progressStart "Exportando Bones:"
	
	bonecount=0
	for i in bonecont do
	(
		progressUpdate (100*bonecount/bonecont.count)	
		
		chunkheader i "BONE"
		
		bonecount+=1			
	)	
	progressEnd()
	
	-----------------------------------------------	
	-- 			CONTROLADORES
	-----------------------------------------------	
	writestring_withoutnull file "CTRL"

	progressStart "Exportando tracks de animacion:"
	
	--Recorro todos los controladores
	controllerscount=0
	for i in totalcontrollers do --Position Rotation Scale
	(
		progressUpdate (100*controllerscount/totalcontrollers.count)
		case i[2] of
		(	
			"position":	(		
							--Position							
							WriteByte file 0x00 #unsigned --position
							WriteLong file controllerscount #signed --Id
							WriteLong file i[3] #signed --Owner
										
							j=i[1] --j has the controller
							WriteLong file j.keys.count #signed
							WriteByte file 0x01 #unsigned					
							for k in j.keys do
							(
								WriteLong file (k.time as integer) #signed
								WriteFloat file k.value.x
								WriteFloat file k.value.z
								WriteFloat file k.value.y
								
								WriteByte  file 0  #unsigned --11111 (tcb ease in ease out)
								/*
								WriteByte  file 31  #unsigned --11111 (tcb ease in ease out)								
								
								WriteFloat file k.tension
								WriteFloat file k.continuity
								WriteFloat file k.bias
								WriteFloat file k.easeTo
								WriteFloat file k.easeFrom
								*/
							)
		
						)
			"rotation": (
							--Rotation
							WriteByte file 0x01 #unsigned --rotation
							WriteLong file controllerscount #signed --Id
							WriteLong file i[3] #signed --Owner

							j=i[1]
							WriteLong file j.keys.count #signed
							WriteByte file 0x01 #unsigned									
							for k in j.keys do
							(
								WriteLong file (k.time as integer) #signed
								rot_temp=inverse (k.value as quat)--Rotation order is different
								WriteFloat file rot_temp.x
								WriteFloat file rot_temp.z
								WriteFloat file rot_temp.y
								WriteFloat file rot_temp.w
								
								WriteByte  file 0  #unsigned --11111 (tcb ease in ease out)
								/*
								WriteByte  file 31 #unsigned --11111 (tcb ease in ease out)
								
								WriteFloat file k.tension
								WriteFloat file k.continuity
								WriteFloat file k.bias
								WriteFloat file k.easeTo
								WriteFloat file k.easeFrom
								*/
							)		
			
						)
			"scale":(
						--Scale
						WriteByte file 0x02 #unsigned --scale
						WriteLong file controllerscount #signed --Id
						WriteLong file i[3] #signed --Owner
						
						j=i[1]
						WriteLong file j.keys.count #signed
						WriteByte file 0x01 #unsigned								
						for k in j.keys do
						(
							WriteLong file (k.time as integer) #signed
							WriteFloat file k.value.x
							WriteFloat file k.value.z
							WriteFloat file k.value.y				
							
							WriteFloat file 0.0  --4 bytes wasted for a quaternion not used xD
							WriteFloat file 0.0  --4 bytes wasted for a quaternion not used xD
							WriteFloat file 0.0  --4 bytes wasted for a quaternion not used xD
							WriteFloat file 0.0  --4 bytes wasted for a quaternion not used xD
							
							WriteByte  file 0  #unsigned --11111 (tcb ease in ease out)							
							/*
							WriteByte  file 31 #unsigned --11111 (tcb ease in ease out)
														
							WriteFloat file k.tension
							WriteFloat file k.continuity
							WriteFloat file k.bias
							WriteFloat file k.easeTo
							WriteFloat file k.easeFrom
							*/
						)
						
					)
			"fov":  (
						--fov
						WriteByte file 0x03 #unsigned --fov
						WriteLong file controllerscount #signed --Id
						WriteLong file i[3] #signed --Owner
						j=i[1]
						WriteLong file j.keys.count #signed
						WriteByte file 0x01 #unsigned													
						for k in j.keys do
						(
							WriteLong file (k.time as integer) #signed
							WriteFloat file k.value
					
							WriteByte  file 0  #unsigned --11111 (tcb ease in ease out)
							/*
							WriteByte  file 31 #unsigned --11111 (tcb ease in ease out)
					
							WriteFloat file k.tension
							WriteFloat file k.continuity
							WriteFloat file k.bias
							WriteFloat file k.easeTo
							WriteFloat file k.easeFrom
							*/
						)
					)
			"roll":  (
						--roll
						WriteByte file 0x04 #unsigned --roll
						WriteLong file controllerscount #signed --Id
						WriteLong file i[3] #signed --Owner
						
						j=i[1]
						WriteLong file j.keys.count #signed
						WriteByte file 0x01 #unsigned													
						for k in j.keys do
						(
							WriteLong file (k.time as integer) #signed
							WriteFloat file k.value
					
							WriteByte  file 0  #unsigned --11111 (tcb ease in ease out)
							/*
							WriteByte  file 31 #unsigned --11111 (tcb ease in ease out)
					
							WriteFloat file k.tension
							WriteFloat file k.continuity
							WriteFloat file k.bias
							WriteFloat file k.easeTo
							WriteFloat file k.easeFrom
							*/
						)
					)					
		)
		controllerscount+=1						
	)
	progressEnd()
	
	
	--Delete TurnMesh Modifier
	for i in geometrycont do
	(
		deleteModifier i 1
	)
	
	fclose file
)--if file !=undefined
)--undo off
)--Bloque final del script