#include <iostream>
#include <vector>

// Point structure representing a 2D point
struct Point
{
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

// Boundary structure representing a rectangular boundary
struct Boundary
{
    double x, y;          // Center x and y coordinates
    double width, height; // Half-size width and height

    Boundary(double x, double y, double width, double height)
        : x(x), y(y), width(width), height(height) {}

    // Check if this boundary contains a point
    bool contains(const Point &point) const
    {
        return (point.x >= x - width && point.x <= x + width &&
                point.y >= y - height && point.y <= y + height);
    }

    // Check if this boundary intersects with another boundary
    bool intersects(const Boundary &range) const
    {
        return !(range.x - range.width > x + width ||
                 range.x + range.width < x - width ||
                 range.y - range.height > y + height ||
                 range.y + range.height < y - height);
    }
};

// QuadTree class representing a quadtree for spatial indexing
class QuadTree
{
    Boundary boundary;
    int capacity;
    std::vector<Point> points;
    bool divided;
    QuadTree *northwest, *northeast, *southwest, *southeast;

public:
    QuadTree(Boundary boundary, int capacity = 4)
        : boundary(boundary), capacity(capacity), divided(false),
          northwest(nullptr), northeast(nullptr), southwest(nullptr), southeast(nullptr) {}

    // Subdivide this QuadTree node into four child nodes
    void subdivide()
    {
        double x = boundary.x;
        double y = boundary.y;
        double w = boundary.width / 2;
        double h = boundary.height / 2;

        northwest = new QuadTree(Boundary(x - w, y - h, w, h), capacity);
        northeast = new QuadTree(Boundary(x + w, y - h, w, h), capacity);
        southwest = new QuadTree(Boundary(x - w, y + h, w, h), capacity);
        southeast = new QuadTree(Boundary(x + w, y + h, w, h), capacity);

        divided = true;
    }

    // Insert a point into the QuadTree
    bool insert(const Point &point)
    {
        if (!boundary.contains(point))
        {
            return false;
        }

        if (points.size() < capacity)
        {
            points.push_back(point);
            return true;
        }
        else
        {
            if (!divided)
            {
                subdivide();
            }
            if (northwest->insert(point) || northeast->insert(point) ||
                southwest->insert(point) || southeast->insert(point))
            {
                return true;
            }
        }
        return false;
    }

    // Query the QuadTree for points within a given range
    void query(const Boundary &range, std::vector<Point> &foundPoints) const
    {
        if (!boundary.intersects(range))
        {
            return;
        }

        for (const Point &point : points)
        {
            if (range.contains(point))
            {
                foundPoints.push_back(point);
            }
        }

        if (divided)
        {
            northwest->query(range, foundPoints);
            northeast->query(range, foundPoints);
            southwest->query(range, foundPoints);
            southeast->query(range, foundPoints);
        }
    }
};

int main()
{
    // Define boundary for the QuadTree
    Boundary boundary(0, 0, 10, 10);
    QuadTree qt(boundary);

    // Insert some points into the QuadTree
    qt.insert(Point(1, 1));
    qt.insert(Point(2, 5));
    qt.insert(Point(-3, 4));
    qt.insert(Point(4, -2));
    qt.insert(Point(-4, -3));

    // Prompt user for the query range
    double centerX, centerY, rangeWidth, rangeHeight;
    std::cout << "Enter the center x-coordinate of the range: ";
    std::cin >> centerX;
    std::cout << "Enter the center y-coordinate of the range: ";
    std::cin >> centerY;
    std::cout << "Enter the width (half-size) of the range: ";
    std::cin >> rangeWidth;
    std::cout << "Enter the height (half-size) of the range: ";
    std::cin >> rangeHeight;

    // Define the user-specified query range
    Boundary range(centerX, centerY, rangeWidth, rangeHeight);
    std::vector<Point> foundPoints;

    // Query the QuadTree with the specified range
    qt.query(range, foundPoints);

    // Print found points within the query range
    std::cout << "Points found in range:\n";
    for (const Point &p : foundPoints)
    {
        std::cout << "(" << p.x << ", " << p.y << ")\n";
    }

    return 0;
}
