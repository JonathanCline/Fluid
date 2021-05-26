#include "ECS/ECS.h"

#include "Utility/Pimpl.h"

namespace fluid
{
	class AbstractArt;
	namespace impl
	{
		void delete_art(AbstractArt*& _art);
	};

	class Art
	{
	public:
		void draw();
		ipimpl_ptr<AbstractArt, impl::delete_art> art{ nullptr };
	};

	class Art_ComponentSystem : public ComponentSystem<Art>
	{
	public:
		void update() override;
	};

	template <>
	struct component_system_type<Art>
	{
		using type = Art_ComponentSystem;
	};
};
