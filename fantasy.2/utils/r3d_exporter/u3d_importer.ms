(--Bloque del script

--Cabecera

signature="UTO!"
filesignature=""--Con la que comprobamos el tipo de fichero
majorversion
minorversion


--Información de fichero
firstframe
lastframe
framerate
deltatime
globalscale
nobjects
ncameras
nlights
nmaterials
ncontrollers
nbones

o_id=#() --Id del objeto(camara,luz,malla) en el .u3d
o_parentid=#() --Id del parent(camara,luz,malla) en el .u3d
o_scene_inode=#() --Inode del max(camara,luz,malla) en la escena
o_material_id=#() -- Materiales de los objetos
s_materials=#() --Materiales de la escena
o_withskin_id=#() --Objetos con skin. Trio de valores #(objeto,tipodeskinning,indice)

s_boneid_single= #()--Lista de BoneId asignado a cada vertice de cada objeto de la escena
s_boneid_weighted=#()--Lista de BoneId asignados a cada vertice de cada objeto de la escena
s_bone_weights=#()--Pesos asignados a cada vértice de cada objeto de la escena
s_bone_list=#()--Lista los bones que usa cada objeto de un escenario
s_bones_id=#()--Lista de BoneId en la escena para acelerar el lookup posterior al asignar el skin
s_scene_bones_inode=#()--Lista de inodes BoneId para acelerar el lookup posterior al asignar el skin


undo off(
--Cargo el archivo 
--Open File dialog
--Open binary read only
filename=getOpenFileName caption:"Open .u3d"  types:"ultimate3d(*.u3d)|*.u3d"

if(filename!=undefined) then -- Abrimos el fichero
(
	disableSceneRedraw()
	file=fopen filename "rb" 
	-----------------------------------------------	
	-- 			FUNCIONES BASE
	-----------------------------------------------	
	o_position=point3 0.0 0.0 0.0--Posicion del objeto a tratar POINT3. Tambien como [0,0,0]
	o_rotation=quat 0.0 0.0 0.0 0.0--Quaternion de rotacion del objeto a tratar 
	o_scale=point3 0.0 0.0 0.0--Escala del objeto a tratar
	o_pivot=point3 0.0 0.0 0.0--Pivote del objeto a tratar
	
	key_tension=25.0
	key_continuity=25.0
	key_bias=25.0
	key_Easein=0.0
	key_Easeout=0.0
	
	fn matrix_transform= 
	(	
		o_position.x=ReadFloat file
		o_position.z=ReadFloat file --z
		o_position.y=ReadFloat file --y

		o_rotation.x=-(ReadFloat file)
		o_rotation.z=-(ReadFloat file) --z
		o_rotation.y=-(ReadFloat file) --y
		o_rotation.w=(ReadFloat file)
	    	
	
		o_scale.x=ReadFloat file
		o_scale.z=ReadFloat file --z
		o_scale.y=ReadFloat file --y
			
	
		o_pivot.x=ReadFloat file
		o_pivot.z=ReadFloat file --z
		o_pivot.y=ReadFloat file --y
		
		--format "Position:%  Pivot:% \n"  o_position o_pivot

	)
	
	fn matrix_reset nodeid=
	(
/*
		nodeid.objectOffsetRot=(quat 0 0 0 0)
		nodeid.objectOffsetScale=[1,1,1]
		nodeid.objectOffsetPos=[0,0,0]*/
	)
	
	fn tcb_controllers nodeid=
	(
		nodeid.pos.controller = tcb_position ()
		if( (classof nodeid) !=TargetDirectionallight and (classof nodeid) !=targetSpot) then
		(
			nodeid.rotation.controller = tcb_rotation ()
		)
		nodeid.scale.controller = tcb_scale ()
		if(classOf nodeid==Targetcamera) then
		(
			nodeid.fov.controller= tcb_float()
		)
	)		
	
	fn set_node_matrix nodeid=
	(
		
		/*
		nodeid.transform=(matrix3 [1,0,0] [0,1,0] [0,0,1] [0,0,0])--Asigno una matriz identidad 			
		
		trans=pretranslate nodeid.transform o_position
		rota=prerotate nodeid.transform inverse(o_rotation)
		scal=prescale nodeid.transform o_scale
		nodeid.transform=(scal*rota*trans)
		
		in coordsys local
		(
			obj.rotation*=RotValInv 
			obj.objectoffsetrot*=RotValInv 
			obj.objectoffsetpos*=RotValInv 
		)
		*/
		
		/*
		nodeid.rotation=o_rotation	
		nodeid.scale=o_scale
		nodeid.position=o_position	
		--format "position object %: % \n" nodeid.name o_position
		nodeid.pivot=o_pivot
		*/
		nodeid.controller.scale=o_scale
		nodeid.position=o_pivot
		if( (classof nodeid) !=TargetDirectionallight and (classof nodeid) !=targetSpot) then
		(
			nodeid.controller.rotation=inverse(o_rotation)
		)
		nodeid.pivot=o_pivot --#it's the same as position 0,0,0 because fantasy is fucked up :D
		--format "position object %: %     pivot:% \n" nodeid.name o_position o_pivot

	
	)
	
	fn get_key_flags=
	(
	
		key_flags=ReadByte file #unsigned
		if(bit.get key_flags 1) then --Leemos tension
		(
			key_tension=ReadFloat file
		)
		if(bit.get key_flags 2) then --Leemos continuity
		(
			key_continuity=ReadFloat file
		)
		if(bit.get key_flags 3) then --Leemos bias
		(
			key_bias=ReadFloat file
		)
		if(bit.get key_flags 4) then --Leemos Easein
		(
			key_Easein=ReadFloat file
		)								
		if(bit.get key_flags 5) then --Leemos Easeout
		(
			key_Easeout=ReadFloat file
		)
	)									
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	
	-----------------------------------------------	
	
	for i=1 to signature.count do
	(
		filesignature+=bit.intaschar (ReadByte file #unsigned)
	)
	
	-----------------------------------------------	
	-- 			DESTRIPE DEL FORMATO
	-----------------------------------------------	
	if(filesignature!=signature) then --Formato desconocido
	(
		print "Formato desconocido"
	)
	
	else -- Empezamos a parsear
	(
		majorversion=ReadByte file #unsigned
		minorversion=ReadByte file #unsigned	
						
		--Salto 4 bytes string (MAIN)
		fseek file 4 #seek_cur
		
		firstframe=ReadLong file #signed
		lastframe=ReadLong file #signed
		framerate=ReadLong file #signed
		deltatime=ReadLong file #signed
		globalscale=ReadFloat file
		nobjects=ReadLong file #signed
		ncameras=ReadLong file #signed
		nlights=ReadLong file #signed
		nmaterials=ReadLong file #signed	
		ncontrollers=ReadLong file #signed		
		nbones=ReadLong file #signed		
		
		sliderTime=firstframe
		
		/*
		format "Majorversion:% Minorversion:% \n" majorversion minorversion
		format "Firstframe:% \n" firstframe
		format "LastFrame:% \n" lastframe
		format "Framerate:% \n" framerate
		format "Deltatime:% \n" deltatime
		format "Globalscale:% \n\n" globalscale
		
		format "Number of objects:% \n" nobjects
		format "Number of cameras:% \n" ncameras
		format "Number of lights:% \n" nlights
		format "Number of materials:% \n" nmaterials
		format "Number of controllers/tracks:% \n" ncontrollers	
	*/

	-----------------------------------------------	
	-- 			MESH 
	-----------------------------------------------	
		--Salto 4 bytes string (MESH)
		fseek file 4 #seek_cur
	
		
		progressStart "Importando Geometria:"
		
		current_boneobject_index=1
		--Recorro Todos los objetos y los voy construyendo
		for i=1 to nobjects do
		(
			progressUpdate (100*i/nobjects)	
			o_name=ReadString file
			append o_id (ReadLong file #signed)
			append o_parentid (ReadLong file #signed)
			o_hidden=ReadByte file #unsigned	--De momento no lo soporto
			--format "Object:% " o_name
			matrix_transform()	--Calculo las componentes de la matriz de transformacion del objeto
			
			o_flags=ReadLong file #signed
			
			o_nvertices=ReadLong file #signed
			o_nfaces=ReadLong file #signed
			o_uvs=ReadLong file #signed
			o_vertexcolor=ReadLong file #signed
			
			o_vert_array = #() 
			o_face_array = #() 
			o_edges_visibility = #() --Visibilidad de cada eje de la cara
			o_uv_array = #()
			o_vertexcolor_array = #()
			o_vertexcolor_faces= #() --Correspondencias vertexcolor faces
			o_face_t= #() --Correspondencias uv coords con uvfaces
			o_skin=false
			o_skin_type=undefined
			
			if((bit.get o_flags 3)==true) then -- skinned object
			(
				o_skin=true
				o_skin_type=ReadByte file #unsigned
				s_bone_list[current_boneobject_index]=#()
				if(o_skin_type==0) then
				(					
					index=s_boneid_single.count+1
					s_boneid_single[index]=#()
					for j=1 to o_nvertices do
					(				
						actual_boneid=ReadLong file #signed
						append s_boneid_single[index][j] actual_boneid
						isalready=findItem s_bone_list[current_boneobject_index] actual_boneid --Check if this bone is already in our bones per object list
						if(isalready==0) then
						(
							append s_bone_list[current_boneobject_index] actual_boneid
						)
					)
				)
				else
				(
					o_bone_number=#()
					index_id=s_boneid_weighted.count+1
					s_boneid_weighted[index_id]=#()
					for j=1 to o_nvertices do
					(
						append o_bone_number (ReadLong file #signed)
					)
					for j=1 to o_nvertices do
					(
						bones_of_vertex=#()					
						for k=1 to o_bone_number[j] do
						(																	
							actual_boneid=ReadLong file #signed
							append bones_of_vertex actual_boneid
							isalready=findItem s_bone_list[current_boneobject_index] actual_boneid --Check if this bone is already in our bones per object list
							if(isalready==0) then
							(
								append s_bone_list[current_boneobject_index] actual_boneid
							)
						)									
						append s_boneid_weighted[index_id] bones_of_vertex
					)	
					index_weight=s_bone_weights.count+1
					s_bone_weights[index_weight]=#()
					for j=1 to o_nvertices do
					(
						weights_of_vertex=#()
						for k=1 to o_bone_number[j] do
						(
							append weights_of_vertex (ReadFloat file)
						)
						append s_bone_weights[index_weight] weights_of_vertex
					)										
					current_boneobject_index+=1					
				)
			)		
			--Coordenadas Vertices
			for j=1 to o_nvertices do
			(
				o_vertex_pos=point3 0.0 0.0 0.0--Posicion del vertice a tratar
				o_vertex_pos.x=ReadFloat file
				o_vertex_pos.z=ReadFloat file --z
				o_vertex_pos.y=ReadFloat file --y
				append o_vert_array o_vertex_pos	

			)
			--Coordenadas de mapeo uv
			for j=1 to o_uvs do
			(
				o_uv_pos=point3 0.0 0.0 0.0--Posicion de las coordenadas uv a tratar
				o_uv_pos.x=ReadFloat file
				o_uv_pos.y=ReadFloat file			
				append o_uv_array o_uv_pos
			)

			--Coordenadas de vertex color
			for j=1 to o_vertexcolor do
			(
				o_vertexcolor_pos=color 0.0 0.0 0.0 --Color rgb del vertex color a tratar
				o_vertexcolor_pos.r=ReadFloat file
				o_vertexcolor_pos.g=ReadFloat file			
				o_vertexcolor_pos.b=ReadFloat file
				o_vertexcolor_pos*=255.0 --Fantasy uses 0-1 color range		
				append o_vertexcolor_array o_vertexcolor_pos
				
			)				
			--Vertices de cada cara
			for j=1 to o_nfaces do
			(
				o_face_indexes=point3 0 0 0 --Indices de los vertices de la cara a tratar
				o_face_indexes.x=ReadLong file #signed --Indice 1
				o_face_indexes.y=ReadLong file #signed --Indice 2
				o_face_indexes.z=ReadLong file #signed --Indice 3	
				
				--Tengo que añadir uno a los indices debido a que 3dsmax solo tiene en cuenta los indices a partir de 1
				o_face_indexes+=1
				append o_face_array o_face_indexes
			)
			--texture coord-vertex assignments
			if(o_uvs>0) then
			(
				for j=1 to o_nfaces do
				(				
					o_face_tep=point3 0 0 0 --Indices de los vertices de la cara a tratar
					o_face_tep.x=ReadLong file #signed --Indice 1
					o_face_tep.y=ReadLong file #signed --Indice 2
					o_face_tep.z=ReadLong file #signed --Indice 3						
					--Tengo que añadir uno a los indices debido a que 3dsmax solo tiene en cuenta los indices a partir de 1
					o_face_tep+=1
					append o_face_t o_face_tep
				)
			)
			if(o_vertexcolor > 0) then
			(
				--vertex color-face assignments
				for j=1 to o_nfaces do
				(
					o_vertexcolor_tep=point3 0 0 0
					o_vertexcolor_tep.x=ReadLong file #signed
					o_vertexcolor_tep.y=ReadLong file #signed	
					o_vertexcolor_tep.z=ReadLong file #signed
					--Tengo que añadir uno a los indices debido a que 3dsmax solo tiene en cuenta los indices a partir de 1
					o_vertexcolor_tep+=1
					append o_vertexcolor_faces o_vertexcolor_tep
				)
			)
			--Visibilidad de los ejes de las caras
			for j=1 to o_nfaces do
			(
				append o_edges_visibility (ReadByte file #unsigned)
			)
			
			--De momento solo soportaré un material id por objeto. Asi pues, leo el primer id y salto el resto			
			material_id=ReadLong file #signed
			--Salto 4*(nfaces-1) bytes porque no me interesa el material id de cada cara
			fseek file (4*(o_nfaces-1)) #seek_cur

			if(bit.get o_flags 1) then --Existe Bounding sphere asociada al objeto
			(
				fseek file 16 #seek_cur --Salto de momento esa informacion
			)
				
			o_mesh = mesh vertices:o_vert_array faces:o_face_array tverts:o_uv_array
			
			append o_scene_inode o_mesh.inode.handle--Añado el id de max del objeto
			
			for j=1 to o_mesh.numfaces do 
			( 
				setEdgeVis o_mesh j 1 (bit.get o_edges_visibility[j] 1)--Bit 1
				setEdgeVis o_mesh j 2 (bit.get o_edges_visibility[j] 2)--Bit 2
				setEdgeVis o_mesh j 3 (bit.get o_edges_visibility[j] 4)--Bit 3
			) 	
					
			--Build TVFaces
			if(o_mesh.numtverts>0) then --Solo si tiene vertices uv
			(
				buildTVFaces o_mesh 
				for j=1 to o_nfaces do 
				(
					setTVFace o_mesh j o_face_t[j]
				)
			)
			--Put Vertex Colors	
			if(o_vertexcolor>0) then
			(
				--o_mesh.showVertexColors=on
				setNumCPVVerts o_mesh o_vertexcolor false
				buildVCFaces o_mesh false
			)
			--Load the vertex colors and their face-index topology
			for j=1 to o_vertexcolor do
			(
				setVertColor o_mesh j o_vertexcolor_array[j]
			)			
			for j=1 to o_vertexcolor_faces.count do
			(				
				setVCFace o_mesh j o_vertexcolor_faces[j]
			)

			update o_mesh 	
								
			o_mesh.name=o_name					
			
			tcb_controllers o_mesh --Aplico controladores tcb(por si acaso)
			
			set_node_matrix o_mesh --Aplico la matriz de transformacion
			
			append o_material_id #(o_mesh,material_id)
			
			--if(o_parentid[i]!=0) then matrix_reset o_mesh
			
			if(o_skin) then
			(
				if(o_skin_type==0) then
				(
					append o_withskin_id #(o_mesh,0,s_boneid_single.count)
				)
				else
				(
					append o_withskin_id #(o_mesh,1,s_boneid_weighted.count)
				)
			)
			
		)
		progressEnd()	
		
	)--else inicial

	--enableSceneRedraw()
	-----------------------------------------------	
	-- 			CAMERAS
	-----------------------------------------------	
	--Salto 4 bytes string (CAMS)
	fseek file 4 #seek_cur
	
	progressStart "Importando Camaras:"
	
	for i=1 to ncameras do
	(
		progressUpdate (100*i/ncameras)	
		o_name=ReadString file
		append o_id (ReadLong file #signed)
		append o_parentid (ReadLong file #signed)	
		fseek file 4 #seek_cur --Se que el targetid va estar en orden, no hace falta leerlo pues

		o_hidden=ReadByte file #unsigned	--De momento no lo soporto
		
		matrix_transform()	--Calculo las componentes de la matriz de transformacion del objeto
		
		o_camera_fov=ReadFloat file
		o_near_clip=ReadFloat file
		o_far_clip=ReadFloat file
		
		o_camera=Freecamera()
		o_camera.fov=o_camera_fov 
		o_camera.nearrange=o_near_clip
		o_camera.farrange=o_far_clip
		
		append o_scene_inode o_camera.inode.handle--Añado el id de max del objeto
		
		o_camera.name=o_name
				
		tcb_controllers o_camera --Aplico controladores tcb(por si acaso)
		
		set_node_matrix o_camera --Aplico la matriz de transformacion
		
		--if(o_parentid[nobjects+i]!=0) then matrix_reset o_camera
	)
	
	progressEnd()
	
	-----------------------------------------------	
	-- 			CAMERA TARGETS
	-----------------------------------------------	
	--Salto 4 bytes string (CTGT)
	fseek file 4 #seek_cur
	
	progressStart "Importando Targets de las Camaras:"
	
	for i=1 to ncameras do
	(
		progressUpdate (100*i/ncameras)	
		o_name=ReadString file
		append o_id (ReadLong file #signed)
		append o_parentid (ReadLong file #signed)	
	
		o_hidden=ReadByte file #unsigned	--De momento no lo soporto
		
		matrix_transform()	--Calculo las componentes de la matriz de transformacion del objeto
		
		o_camera=maxOps.getNodeByHandle o_scene_inode[i+nobjects] --Voy accediendo a las camaras
		
		o_camera.type=#target --Las convierto a target
	
		append o_scene_inode o_camera.target.inode.handle--Añado el id de max del objeto
				
		o_camera.target.name=o_name
		
		tcb_controllers o_camera.target --Aplico controladores tcb(por si acaso)
		
		set_node_matrix o_camera.target --Aplico la matriz de transformacion
		
		--if(o_parentid[nobjects+ncameras+i]!=0) then matrix_reset o_camera.target
	)
	
	progressEnd()

	-----------------------------------------------	
	-- 			LUCES
	-----------------------------------------------	
	--Salto 4 bytes string (LITE)
	fseek file 4 #seek_cur
	
	progressStart "Importando Luces:"
	
	for i=1 to nlights do
	(
		progressUpdate (100*i/nlights)	
		o_name=ReadString file
		append o_id (ReadLong file #signed)
		append o_parentid (ReadLong file #signed)
		fseek file 4 #seek_cur
		--append o_id (ReadLong file #signed) --TargetID
		--append o_parentid 0   --El Target no tiene nunca parent

		o_hidden=ReadByte file #unsigned	--De momento no lo soporto
		
		matrix_transform()	--Calculo las componentes de la matriz de transformacion del objeto
		
		o_light_type=ReadLong file #unsigned
		o_light_color=color 0.0 0.0 0.0
		o_light_color.r=ReadFloat file
		o_light_color.g=ReadFloat file
		o_light_color.b=ReadFloat file
		o_light_atten_start=ReadFloat file
		o_light_atten_end=ReadFloat file
		o_light_use_atten=ReadByte file #unsigned
		
		if(o_light_use_atten==1) then
		(
			o_light_use_atten=true
		)
		else
		(
			o_light_use_atten=false
		)
			
		
		o_light_multiplier=ReadFloat file
		case o_light_type of
		(						
			0:	(
					o_light=Omnilight()
					
					append o_scene_inode o_light.inode.handle--Añado el id de max del objeto
					
					o_light.name=o_name
					o_light.rgb=o_light_color*255
					o_light.farAttenStart=	o_light_atten_start
					o_light.farAttenEnd=o_light_atten_end
					o_light.useFarAtten=o_light_use_atten
					o_light.multiplier=o_light_multiplier
					
					tcb_controllers o_light --Aplico controladores tcb(por si acaso)
					
					set_node_matrix o_light --Aplico la matriz de transformacion
					
					--if(o_parentid[nobjects+ncameras*2+i]!=0) then matrix_reset o_light
				)
			1:	(
					o_light=Directionallight()

					o_light.type=#targetDirect
					append o_scene_inode o_light.inode.handle--Añado el id de max del objeto
					append o_scene_inode o_light.target.inode.handle--Añado el id de max del objeto
		
					o_light.target.position.x=ReadFloat file
					o_light.target.position.z=ReadFloat file --z
					o_light.target.position.y=ReadFloat file --y
					
					o_light.name=o_name
					o_light.rgb=o_light_color*255
					o_light.farAttenStart=	o_light_atten_start
					o_light.farAttenEnd=o_light_atten_end
					o_light.useFarAtten=o_light_use_atten
					o_light.multiplier=o_light_multiplier
				
					tcb_controllers o_light --Aplico controladores tcb(por si acaso)
					tcb_controllers o_light.target --Aplico controladores tcb(por si acaso)

					
					set_node_matrix o_light --Aplico la matriz de transformacion
					
					--if(o_parentid[nobjects+ncameras*2+i]!=0) then matrix_reset o_light
				)	
			2:	(
					o_light=freeSpot()

					o_light.type=#targetSpot
					
					append o_scene_inode o_light.inode.handle--Añado el id de max del objeto	
					append o_scene_inode o_light.target.inode.handle--Añado el id de max del objeto

					o_light.target.position.x=ReadFloat file
					o_light.target.position.z=ReadFloat file --z
					o_light.target.position.y=ReadFloat file --y

					o_light.name=o_name
					o_light.rgb=o_light_color*255
					o_light.farAttenStart=	o_light_atten_start
					o_light.farAttenEnd=o_light_atten_end
					o_light.useFarAtten=o_light_use_atten
					o_light.multiplier=o_light_multiplier
					o_light.falloff=(ReadFloat file)*2.0
					o_light.hotspot=(ReadFloat file) + 128

					
					tcb_controllers o_light --Aplico controladores tcb(por si acaso)
					tcb_controllers o_light.target --Aplico controladores tcb(por si acaso)
					
					set_node_matrix o_light --Aplico la matriz de transformacion
					
					--if(o_parentid[nobjects+ncameras*2+i]!=0) then matrix_reset o_light
				)			
						
		)		
	
	)
	progressEnd()	
	
	-----------------------------------------------	
	-- 			MATERIAL IDS
	-----------------------------------------------	
	--Salto 4 bytes string (MATS)
	fseek file 4 #seek_cur
	
	progressStart "Importando Materiales:"
	
	for i=1 to nmaterials do
	(
		progressUpdate (50*i/nmaterials)	
		o_name=ReadString file

		fseek file 4 #seek_cur --Salto el matid pues va en orden y no necesitaremos indexarlo
		
		cur_material=Standardmaterial ()
		cur_material.name=o_name
	
		append s_materials cur_material --Vamos añadiendo materiales al array

		if(i<=meditMaterials.count) then
		(
			meditMaterials[i]=cur_material
		)
	)
	omaterialcount=0
	for j in o_material_id do
	(
		progressUpdate ((50*omaterialcount/o_material_id.count)+50)	 --Voy desde 50 hasta 100 
		
		j[1].material=s_materials[j[2]+1]
		omaterialcount+=1
	)
	/*
	--Recorro todos los objetos de la escena y les asigno sus materiales correspondientes
	for j=1 to nobjects do
	(
		progressUpdate ((50*j/nobjects)+50)	 --Voy desde 50 hasta 100 
		o_mesh=maxOps.getNodeByHandle o_scene_inode[j]
		
		o_mesh.material=s_materials[o_material_id[j]+1] --Sumo mas uno porque empiezan desde 0 los materiales
	)*/
	
	progressEnd()	

	-----------------------------------------------	
	-- 			BONES
	-----------------------------------------------	
	--Salto 4 bytes string (BONE)
	fseek file 4 #seek_cur
	
	--progressStart "Importando skinning:"	
	
	for i=1 to nbones do
	(
		--progressUpdate (10*i/nbones)	
		o_bone_name=ReadString file
		
		bone_id=(ReadLong file #signed) --Para acelerar el lookup posterior al asignar el skin
		append s_bones_id bone_id
		append o_id bone_id
		append o_parentid (ReadLong file #signed)
		o_bone_hidden=ReadByte file #unsigned
	
		matrix_transform()	--Calculo las componentes de la matriz de transformacion del objeto
		
		o_bone=Point()
		o_bone.name=o_bone_name
		append o_scene_inode o_bone.inode.handle--Añado el id de max del objeto		
		append s_scene_bones_inode o_bone.inode.handle--Para acelerar el lookup posterior al asignar el skin
		o_bone.boneEnable=true
		tcb_controllers o_bone --Aplico controladores tcb(por si acaso)
			
		set_node_matrix o_bone --Aplico la matriz de transformacion
	)			
	skinobjnum=1
	for j in o_withskin_id do
	(
		--progressUpdate ((90*skinobjnum/o_withskin_id.count)+10)	
		max modify mode
		select j[1]	
		modPanel.addModToSelection (Skin ()) 
		skinmod=j[1].modifiers[#Skin]
		skinmod.bone_Limit=4
		skinmod.filter_vertices = on
		skinmod.showNoEnvelopes = on
		skinmod.weightAllVertices = on
		
		--format "El bone list %" s_bone_list
		for k in s_bone_list[skinobjnum] do--Bones del objeto
		(
			index=findItem s_bones_id k
			--print (maxOps.getNodeByHandle o_scene_bones_inode[index])
			--append sel_list (maxOps.getNodeByHandle s_scene_bones_inode[index])	
			skinOps.addbone skinmod (maxOps.getNodeByHandle s_scene_bones_inode[index]) 1
		)	
		--Y asigno los pesos

		verts_count=j[1].mesh.numverts
		skinOps.SelectVertices skinmod #{1..verts_count}	
		skinOps.bakeSelectedVerts skinmod --Ready to weight now!

		if(j[2]==0) then --Skin Type==simple
		(
			index=j[3]	
			actual_object_bone_id=s_boneid_single[j[3]]
			orderer_boneids=#()
			for l in actual_object_bone_id do
			(
				index=findItem s_bone_list[skinobjnum] l--Fantasy BoneID != every Skin modifier Bone Ids(They are reordered)
				append orderer_boneids actual_object_bone_id[index]
			)							
			for k=1 to verts_count do
			(
				skinOps.SetVertexWeights skinmod k orderer_boneids[k] 1.0
			)
		)
		/*
		s_boneid_single= #()--Lista de BoneId asignado a cada vertice de cada objeto de la escena
s_boneid_weighted=#()--Lista de BoneId asignados a cada vertice de cada objeto de la escena
s_bone_weights=#()--Pesos asignados a cada vértice de cada objeto de la escena
s_bone_list=#()--Lista los bones que usa cada objeto de un escenario
s_bones_id=#()--Lista de BoneId en la escena para acelerar el lookup posterior al asignar el skin
s_scene_bones_inode=#()--Lista de inodes BoneId para acelerar el lookup posterior al asignar el skin
*/
		else
		(
			actual_object_bone_id=s_boneid_weighted[j[3]]
			actual_object_weights=s_bone_weights[j[3]]
			--print actual_object_bone_id
			--print actual_object_bone_weights
			--print actual_object_bone_id
			unordered_boneids=s_bone_list[skinobjnum]			
			
			for k=1 to verts_count do
			(				
				skinOps.unNormalizeVertex skinmod k true --Unnormalize vertices
				--print actual_object_bone_id[k]
				orderer_boneids=#()
				for l in actual_object_bone_id[k] do
				(
					--format "% %\n" l s_bone_list[skinobjnum]
					index=findItem unordered_boneids l--Fantasy BoneID != every Skin modifier Bone Ids(They are reordered)
					--format "Bone unorderedID:%  Bone orderedID:% actual_boneID:% \n" index l actual_object_bone_id[index]
					append orderer_boneids index--actual_object_bone_id[index]
				)		
				--skinOps.SelectVertices skinmod k
				--skinOps.SetVertexWeights skinmod k orderer_boneids actual_object_weights[k]
				--skinOps.SetVertexWeights skinmod k orderer_boneids actual_object_weights[k]
				--Está completamente roto xD
				--format "skinOps.SetVertexWeights skinmod % % % \n" k orderer_boneids actual_object_weights[k]
				--format "Bones:% Weight:% \n" orderer_boneids actual_object_weights[k]
			)

		)	
		skinobjnum+=1 	
	)
	--progressEnd()	



	-----------------------------------------------	
	-- 			JERARQUIAS
	-----------------------------------------------	
	progressStart "Estableciendo Jerarquias entre objetos:"
	at time firstframe
	(
		print o_scene_inode
		--Recorro todos los id y les aplico la jerarquia correspondiente
		for i=1 to o_id.count do
		(
			progressUpdate (100*i/nobjects)	
			if(o_parentid[i]!=0) then --Si es 0 no tiene padres(el mundo vaya)
			(
				index=findItem o_id o_parentid[i] --Busco el indice .u3d donde esta el parentid que buscamos
				parent=o_scene_inode[index] --Este es el scene_inode del padre del objeto a tratar
				o_node=maxOps.getNodeByHandle o_scene_inode[i] --Selecciono el nodo del objeto a tratar
				o_node.parent=(maxOps.getNodeByHandle parent) --Le asigno su padre
			)
		)
	)--at time
	progressEnd()	

	-----------------------------------------------	
	-- 			CONTROLADORES
	-----------------------------------------------	

	--Salto 4 bytes string (CTRL)
	fseek file 4 #seek_cur
	
	progressStart "Importando tracks de animacion:"
	
	animationRange = interval firstframe lastframe

	--Recorro todos los controladores
	for i=1 to ncontrollers do
	(
		progressUpdate (100*i/ncontrollers)	
		
		ctrl_type=ReadByte file #unsigned
		ctrl_id=ReadLong file #signed
		owner_id=Readlong file #signed --id de .u3d
		ctrl_keyframes=Readlong file #signed
		ctrl_interp=ReadByte file #unsigned
		--Sacamos el inode de escena(el owner id  sin mas no nos vale para mucho)
		
		index=findItem o_id owner_id --Busco el indice .u3d donde esta el propietario del controlador
		--format "% % % % % \n"ctrl_type ctrl_id owner_id ctrl_keyframes ctrl_interp
	
		o_node=maxOps.getNodeByHandle o_scene_inode[index] --Selecciono el objeto con el indice anterior	
		
		case ctrl_interp of
		(	
			--TCB Interpolation Scheme					
			1:	(
					case ctrl_type of
					(
						--Position
						0:	(
								for j=1 to ctrl_keyframes do
								(
									key_time=ReadLong file #signed
									key_position=point3 0.0 0.0 0.0
									key_position.x=ReadFloat file
									key_position.z=ReadFloat file								
									key_position.y=ReadFloat file								
																							
									get_key_flags() --Miro los flag y leo si es preciso	
									
									addNewKey o_node.controller.position.controller (key_time/deltatime)
									o_node.controller.position.controller.keys[j].value=key_position
								)
							)
						
						--Rotation
						1:	(
								for j=1 to ctrl_keyframes do
								(
									key_time=ReadLong file #signed								
									key_rotation=quat 0.0 0.0 0.0 0.0
									key_rotation.x=ReadFloat file
									key_rotation.z=ReadFloat file
									key_rotation.y=ReadFloat file								
									key_rotation.w=ReadFloat file								
									key_rotation=inverse key_rotation
									
									get_key_flags() --Miro los flag y leo si es preciso																		
									
									addNewKey o_node.controller.rotation.controller (key_time/deltatime)
									o_node.controller.rotation.keys[j].value=key_rotation
									
								)
							)
						
						--Scale
						2:	(
								for j=1 to ctrl_keyframes do
								(
									key_time=ReadLong file #signed
									key_scale=point3 0.0 0.0 0.0
									key_scale.x=ReadFloat file
									key_scale.z=ReadFloat file								
									key_scale.y=ReadFloat file
																
									fseek file 16 #seek_cur --Salto el Quaternion fantasma																	
									
									get_key_flags() --Miro los flag y leo si es preciso																		
									
									addNewKey o_node.controller.scale.controller (key_time/deltatime)
									o_node.controller.scale.controller.keys[j].value=key_scale
								)						
							)
							
						--Fov
						3:	(
								o_node.fov.controller= tcb_float()
								for j=1 to ctrl_keyframes do
								(
									key_time=ReadLong file #signed
									key_fov=ReadFloat File
																		
									get_key_flags() --Miro los flag y leo si es preciso																		
																		
									addNewKey o_node.fov.controller (key_time/deltatime)
									o_node.fov.controller.keys[j].value=key_fov
									
								)						
							)
							
						--Roll
						4:	(
								for j=1 to ctrl_keyframes do
								(
									key_time=ReadLong file #signed
									key_roll=ReadFloat file

									get_key_flags() --Miro los flag y leo si es preciso																		
																										
									addNewKey o_node.controller.Roll_Angle.controller (key_time/deltatime)
									o_node.controller.Roll_Angle.controller.keys[j].value=key_roll
									
								)						
							)
					)--Case controller type
					
				)--Case TCB interpolation scheme
		
		)--Case
		
	)--For
	
	progressEnd()	
			
	fclose file
	enableSceneRedraw()
	ForceCompleteRedraw doDisabled:true
)--if file !=undefined
)--undo off
)--Bloque final del script