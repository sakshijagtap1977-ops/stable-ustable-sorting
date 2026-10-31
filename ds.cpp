import streamlit as st
import pandas as pd

# ============================================================
# EMPLOYEE SORTING DEMONSTRATOR
# Streamlit UI: Stable vs Unstable Sorting
# ============================================================

DEFAULT_EMPLOYEES = [
    (101, "Rahul", "IT"),
    (102, "Priya", "HR"),
    (103, "Amit", "IT"),
    (104, "Sneha", "Finance"),
    (105, "Vikas", "HR"),
    (106, "Neha", "IT"),
    (107, "Rohan", "Finance"),
    (108, "Anjali", "HR"),
]


def stable_sort(records):
    """Stable sort by department; equal departments keep their order."""
    return sorted(records, key=lambda employee: employee[2].casefold())


def unstable_selection_sort(records):
    """
    Selection sort with <= tie handling, which can change the order
    of equal-department records. This is a demonstration, not a claim
    that every selection-sort input will visibly become unstable.
    """
    arr = records.copy()
    comparisons = 0
    swaps = 0

    for i in range(len(arr) - 1):
        selected = i
        for j in range(i + 1, len(arr)):
            comparisons += 1
            if arr[j][2].casefold() <= arr[selected][2].casefold():
                selected = j
        if selected != i:
            arr[i], arr[selected] = arr[selected], arr[i]
            swaps += 1

    return arr, comparisons, swaps


def department_order(records):
    order = {}
    for employee_id, name, department in records:
        order.setdefault(department, []).append(name)
    return order


def compare_department_order(original, sorted_records):
    original_order = department_order(original)
    new_order = department_order(sorted_records)
    rows = []

    for department, names in original_order.items():
        updated_names = new_order.get(department, [])
        rows.append({
            "Department": department,
            "Original order": " → ".join(names),
            "Sorted order": " → ".join(updated_names),
            "Result": (
                "ORDER PRESERVED"
                if names == updated_names
                else "ORDER CHANGED"
            ),
        })

    return rows


def records_to_dataframe(records):
    return pd.DataFrame(records, columns=["Employee ID", "Name", "Department"])


st.set_page_config(
    page_title="Employee Sorting Demonstrator",
    page_icon="🔃",
    layout="wide",
)

st.title("🔃 Employee Sorting Demonstrator")
st.caption("Explore stable sorting and selection sort using employee records.")

with st.sidebar:
    st.header("Controls")
    mode = st.radio(
        "Choose an operation",
        [
            "View original records",
            "Stable sorting",
            "Unstable selection sort",
            "Compare both algorithms",
            "Enter your own employee data",
        ],
    )
    st.markdown("---")
    st.write("**Stable sorting:** preserves the original order of records with equal sort keys.")
    st.write("**Selection sort:** the usual swap-based version does not guarantee stability.")

if "employees" not in st.session_state:
    st.session_state.employees = DEFAULT_EMPLOYEES.copy()

employees = st.session_state.employees

if mode == "Enter your own employee data":
    st.subheader("Enter employee records")
    st.write("Add, edit, or remove rows. Department names are used as the sorting key.")

    editable = records_to_dataframe(employees)
    edited = st.data_editor(
        editable,
        num_rows="dynamic",
        use_container_width=True,
        column_config={
            "Employee ID": st.column_config.NumberColumn(
                "Employee ID", min_value=1, step=1, format="%d"
            ),
            "Name": st.column_config.TextColumn("Name", required=True),
            "Department": st.column_config.TextColumn("Department", required=True),
        },
        key="employee_editor",
    )

    col1, col2 = st.columns(2)
    with col1:
        if st.button("Save employee data", type="primary"):
            cleaned = []
            errors = []

            for row_number, row in edited.iterrows():
                name = str(row["Name"]).strip() if pd.notna(row["Name"]) else ""
                department = (
                    str(row["Department"]).strip()
                    if pd.notna(row["Department"])
                    else ""
                )
                emp_id = row["Employee ID"]

                if not name or not department or pd.isna(emp_id):
                    errors.append(f"Row {row_number + 1}: fill in all three fields.")
                    continue

                try:
                    emp_id = int(emp_id)
                except (TypeError, ValueError):
                    errors.append(f"Row {row_number + 1}: Employee ID must be a whole number.")
                    continue

                cleaned.append((emp_id, name, department))

            if errors:
                for error in errors:
                    st.error(error)
            elif not cleaned:
                st.error("Please enter at least one valid employee record.")
            else:
                st.session_state.employees = cleaned
                st.success(f"Saved {len(cleaned)} employee record(s).")
                st.rerun()

    with col2:
        if st.button("Restore sample data"):
            st.session_state.employees = DEFAULT_EMPLOYEES.copy()
            st.rerun()

    st.info("After saving, choose an algorithm from the sidebar to see its result.")

elif mode == "View original records":
    st.subheader("Original employee records")
    st.dataframe(records_to_dataframe(employees), hide_index=True, use_container_width=True)
    st.metric("Total employees", len(employees))

elif mode == "Stable sorting":
    st.subheader("Stable sorting by department")
    result = stable_sort(employees)

    left, right = st.columns(2)
    with left:
        st.markdown("**Before sorting**")
        st.dataframe(records_to_dataframe(employees), hide_index=True, use_container_width=True)
    with right:
        st.markdown("**After stable sorting**")
        st.dataframe(records_to_dataframe(result), hide_index=True, use_container_width=True)

    st.markdown("**Order comparison within each department**")
    st.dataframe(
        pd.DataFrame(compare_department_order(employees, result)),
        hide_index=True,
        use_container_width=True,
    )
    st.success("Python's sorted() is stable: employees with the same department retain their relative order.")

elif mode == "Unstable selection sort":
    st.subheader("Selection sort by department")
    result, comparisons, swaps = unstable_selection_sort(employees)

    left, right = st.columns(2)
    with left:
        st.markdown("**Before sorting**")
        st.dataframe(records_to_dataframe(employees), hide_index=True, use_container_width=True)
    with right:
        st.markdown("**After selection sort**")
        st.dataframe(records_to_dataframe(result), hide_index=True, use_container_width=True)

    m1, m2, m3 = st.columns(3)
    m1.metric("Employees", len(employees))
    m2.metric("Comparisons", comparisons)
    m3.metric("Swaps", swaps)

    st.markdown("**Order comparison within each department**")
    st.dataframe(
        pd.DataFrame(compare_department_order(employees, result)),
        hide_index=True,
        use_container_width=True,
    )
    st.warning(
        "Selection sort is generally unstable because swaps can change the order "
        "of equal-key records. Whether this input visibly changes depends on the data."
    )

elif mode == "Compare both algorithms":
    stable_result = stable_sort(employees)
    selection_result, comparisons, swaps = unstable_selection_sort(employees)

    st.subheader("Algorithm comparison")
    col1, col2 = st.columns(2)

    with col1:
        st.markdown("### Stable sort")
        st.dataframe(records_to_dataframe(stable_result), hide_index=True, use_container_width=True)
        st.caption("Uses Python sorted(), which preserves relative order for equal keys.")

    with col2:
        st.markdown("### Selection sort")
        st.dataframe(records_to_dataframe(selection_result), hide_index=True, use_container_width=True)
        st.caption(f"Comparisons: {comparisons} · Swaps: {swaps}")

    comparison_rows = []
    stable_orders = department_order(stable_result)
    selection_orders = department_order(selection_result)

    for department, names in department_order(employees).items():
        stable_names = stable_orders.get(department, [])
        selection_names = selection_orders.get(department, [])
        comparison_rows.append({
            "Department": department,
            "Original order": " → ".join(names),
            "Stable result": " → ".join(stable_names),
            "Selection sort result": " → ".join(selection_names),
            "Stable order preserved": names == stable_names,
            "Selection order preserved": names == selection_names,
        })

    st.markdown("**Stability comparison**")
    st.dataframe(pd.DataFrame(comparison_rows), hide_index=True, use_container_width=True)

    st.markdown("**Quick explanation**")
    st.write("- A stable algorithm keeps equal-department employees in their original relative order.")
    st.write("- Standard swap-based selection sort is not guaranteed to do this.")
    st.write("- Both algorithms sort records by department; only stability is being compared.")

st.markdown("---")
st.caption("Educational demonstration. Sorting is performed locally by this Streamlit app.")
