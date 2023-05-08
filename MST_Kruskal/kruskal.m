% Import data
x = [10 50 100 150 200 250 300 350 400 450 500 550 600 650 700 750 800 850 900 950 1000];
y = [5 25 57 84 115 151 171 199 224 253 291 311 342 379 399 427 466 489 519 537 573];

% Define objective function
f = @(params) sum((y - (params(1) * x .* log(x) + params(2))).^2);

% Find optimal parameters
params = fminsearch(f, [1, 0]);

% Plot data and approximation
n_values = linspace(min(x), max(x), 1000);
y_approx = params(1) * n_values .* log(n_values) + params(2);
plot(x, y, 'o', n_values, y_approx, 'LineWidth', 2, 'MarkerSize', 8);
grid minor;
xlabel('Ilość wierzchołków w grafie');
ylabel('Czas [ms]');
title('Czas znalezienia MST dla danej ilości wierzchołków - Algorytm Kruskala')
legend('Czas znalezienia MST', 'Przybliżona złożonośc czasowa');
