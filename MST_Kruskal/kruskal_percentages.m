% Import data
y = [386 415 448 459 469 480 460 467 556 591];
x = [10 20 30 40 50 60 70 80 90 100];


% Define objective function
f = @(params) sum((y - (params(1) * x .* log(x) + params(2))).^2);

% Find optimal parameters
params = fminsearch(f, [1, 0]);

% Plot data and approximation
n_values = linspace(min(x), max(x), 1000);
y_approx = params(1) * n_values .* log(n_values) + params(2);
plot(x, y, 'o', n_values, y_approx, 'LineWidth', 2, 'MarkerSize', 8);
grid minor;
xlabel('Procent usuniętych krawędzi w grafie [%]');
ylabel('Czas [ms]');
title('Czas znalezienia MST dla kolejno usuwanych krawędzi - Algorytm Kruskala')
legend('Czas znalezienia MST', 'Przybliżona złożonośc czasowa');
