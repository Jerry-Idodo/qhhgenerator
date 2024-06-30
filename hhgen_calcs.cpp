#include "hhgen_calcs.h"
#include <QDate>
#include <vector>


static int get_opening_time(const std::string& str)
{
    int ret = -1;
    for (int i = 0; i < max_hours; i++ ) {
        if (str == HalfHourlyTimes[i]) {
            ret = i;
        }
    }
    return ret;
}

static bool get_weekly_opening_time(const std::vector<std::vector<std::string>>& OpenTimesStr,
                                    std::vector<std::vector<int>>& OpenTimes)
{
    int temp;
    for (const std::vector<std::string>& sv: OpenTimesStr) {
        std::vector<int> temp_vec;
        for (const std::string& s: sv) {
            temp = get_opening_time(s);
            if (temp == -1)
                return false;
            temp_vec.push_back(temp);
        }
        OpenTimes.push_back(temp_vec);
    }
    return true;
}

static void get_peak_hours(double& peak_hours,
                           double& offpeak_hours,
                           const std::vector<QDate> Dates,
                           const std::vector<std::vector<int>>& OpenTimes)
{
    const int weekday = Dates[0].dayOfWeek();
    offpeak_hours = max_hours - OpenTimes[weekday][1] + OpenTimes[weekday][0];
    offpeak_hours += 52 * (max_hours - OpenTimes[1][1] + OpenTimes[1][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[2][1] + OpenTimes[2][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[3][1] + OpenTimes[3][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[4][1] + OpenTimes[4][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[5][1] + OpenTimes[5][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[6][1] + OpenTimes[6][0]);
    offpeak_hours += 52 * (max_hours - OpenTimes[0][1] + OpenTimes[0][0]);
    peak_hours = 7 * 48 * 52 + 48 - offpeak_hours;
}

void fill_peak_offpeak_data(std::vector<std::vector<double>>& HHData,
                            const double peak_usage,
                            const double offpeak_usage,
                            std::vector<QDate>& Dates,
                            std::vector<std::vector<int>>OpenTimes)
{
    int start_weekday = Dates[0].dayOfWeek();
    int days_in_year = Dates[0].addYears(1).toJulianDay() - Dates[0].toJulianDay();
    for (int i = 0; i < days_in_year; i++) {
        int w = (i + start_weekday) % 7;
        int j = 0;
        std::vector<double> buff;

        if (i != 0)
            Dates.push_back(Dates[0].addDays(i));

        for (j = 0; j <= OpenTimes[w][0]; j++) {
            buff.push_back(offpeak_usage);
        }
        for (j = OpenTimes[w][0] + 1; j <= OpenTimes[w][1]; j++) {
            buff.push_back(peak_usage);
        }
        for (j = OpenTimes[w][1] + 1; j < max_hours; j++) {
            buff.push_back(offpeak_usage);
        }
        HHData.push_back(buff);
    }

}

void gen_peak_offpeak_data(std::vector<std::vector<double>>& HHData,
                           std::vector<QDate>& Dates,
                           const std::vector<std::vector<std::string>>& OpenTimesStr,
                           const double annual_usage,
                           const double baseload,
                           bool *ok)
{
    double peak_hours, offpeak_hours;
    std::vector<std::vector<int>> OpenTimes;

    if (!(get_weekly_opening_time(OpenTimesStr, OpenTimes))) {
        if (ok != nullptr)
            *ok = false;
        return;
    }

    get_peak_hours(peak_hours, offpeak_hours, Dates, OpenTimes);
    const double peak_usage = annual_usage / (peak_hours + offpeak_hours * (baseload/100.0));
    const double offpeak_usage = annual_usage / (offpeak_hours + peak_hours * (100.0/baseload));

    fill_peak_offpeak_data(HHData, peak_usage, offpeak_usage, Dates, OpenTimes);
    if (ok != nullptr)
        *ok = true;
}
