#include "Scene/ScriptableEntity.h"
#include "Core/Input.h"
#include <vector>

namespace Script
{
    class MazeGenerator : public ScriptableEntity
    {
    public:
        // TODO put this somewhere all scripts have access to
        // Add force to rb
        glm::vec3 right = glm::vec3(1, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 forward = glm::vec3(0, 0, 1);

        int maxRow = 10;
        int maxCol = 10;
        std::vector<glm::vec3> points;

    public:
        void Start()
        {
            // Generating positions
            for (int i = 0; i < maxRow; ++i)
            {
                for (int j = 0; j < maxCol; ++j)
                {
                    points.emplace_back(glm::vec3(i*2.5, j*2.5, 0.0f));
                }
            }

            // Instantiating object at those positions
            for (int i = 0; i < points.size(); ++i)
            {
                Instantiate(entity, points[i]);
            }
        }

        void Update(float dt)
        {
        }

    };
}
