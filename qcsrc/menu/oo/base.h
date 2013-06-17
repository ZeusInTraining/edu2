.string classname;
.entity superclass;

entity spawnObject()
{
	entity e;
	e = spawn();
	e.classname = "Object";
	e.superclass = null_entity;
	return e;
}
