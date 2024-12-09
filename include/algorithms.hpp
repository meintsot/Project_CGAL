int find_best_method(CDT cdt, Face_handle face);

void local_search(CDT& cdt, std::vector<Point>& steinerPoints, int L);

double calculateEnergy(const CDT& cdt, double a, double b, const std::vector<Point> steinerPoints);

double randomProbability();

void simulated_annealing(CDT& cdt, std::vector<Point>& steinerPoints, double a, double b, int L);

void evaluate_method(TriangulationMethod* method, double a, double b, int obtuseCountOld, int obtuseCountNew, std::vector<Point>& steinerPoints, double previousEnergy, CDT& newCdt);

void update_pheromones(TriangulationMethod* method, double pheromonesEvaporation);

void ant_colonies(CDT& cdt, std::vector<Point>& steinerPoints, double a, double b, double x , double y, double lambda, double kappa, int L);